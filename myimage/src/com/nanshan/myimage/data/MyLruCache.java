package com.nanshan.myimage.data;

import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Map.Entry;

/**
 * A cache that holds strong references to a limited number of values. Each time
 * a value is accessed, it is moved to the head of a queue. When a value is
 * added to a full cache, the value at the end of that queue is evicted and may
 * become eligible for garbage collection.
 * Cache保存一个强引用来限制内容数量，每当Item被访问的时候，此Item就会移动到队列的头部。
 * 当cache已满的时候加入新的item时，在队列尾部的item会被回收。
 * <p>
 * If your cached values hold resources that need to be explicitly released,
 * override {@link #entryRemoved}. 如果你cache的某个值需要明确释放，重写entryRemoved()
 * <p>
 * If a cache miss should be computed on demand for the corresponding keys,
 * override {@link #create}. This simplifies the calling code, allowing it to
 * assume a value will always be returned, even when there's a cache miss.
 * 如果key相对应的item丢掉啦，重写create().这简化了调用代码，即使丢失了也总会返回。
 * <p>
 * By default, the cache size is measured in the number of entries. Override
 * {@link #sizeOf} to size the cache in different units. For example, this cache
 * is limited to 4MiB of bitmaps: 默认cache大小是测量的item的数量，重写sizeof计算不同item的 大小。
 * 
 * <pre>
 * {@code 
 *   int cacheSize = 4 * 1024 * 1024; // 4MiB 
 *   LruCache<String, Bitmap> bitmapCache = new LruCache<String, Bitmap>(cacheSize) { 
 *       protected int sizeOf(String key, Bitmap value) { 
 *           return value.getByteCount(); 
 *       } 
 *   }}
 * </pre>
 * 
 * <p>
 * This class is thread-safe. Perform multiple cache operations atomically by
 * synchronizing on the cache:
 * 
 * <pre>
 * {@code 
 *   synchronized (cache) { 
 *     if (cache.get(key) == null) { 
 *         cache.put(key, value); 
 *     } 
 *   }}
 * </pre>
 * 
 * <p>
 * This class does not allow null to be used as a key or value. A return value
 * of null from {@link #get}, {@link #put} or {@link #remove} is unambiguous:
 * the key was not in the cache. 不允许key或者value为null
 * 当get（），put（），remove（）返回值为null时，key相应的项不在cache中
 */
public class MyLruCache<K, V> {
	private final LinkedHashMap<K, V> map;

	/** Size of this cache in units. Not necessarily the number of elements. */
	private int size; // 已经存储的大小
	private int maxSize; // 规定的最大存储空间

	private int putCount; // put的次数
	private int createCount; // create的次数
	private int evictionCount; // 回收的次数
	private int hitCount; // 命中的次数
	private int missCount; // 丢失的次数

	/**
	 * @param maxSize
	 *            for caches that do not override {@link #sizeOf}, this is the
	 *            maximum number of entries in the cache. For all other caches,
	 *            this is the maximum sum of the sizes of the entries in this
	 *            cache.
	 */
	public MyLruCache(int maxSize) {
		if (maxSize <= 0) {
			throw new IllegalArgumentException("maxSize <= 0");
		}
		this.maxSize = maxSize;
		this.map = new LinkedHashMap<K, V>(0, 0.75f, true);
	}

	/**
	 * Returns the value for {@code key} if it exists in the cache or can be
	 * created by {@code #create}. If a value was returned, it is moved to the
	 * head of the queue. This returns null if a value is not cached and cannot
	 * be created. 通过key返回相应的item，或者创建返回相应的item。相应的item会移动到队列的头部，
	 * 如果item的value没有被cache或者不能被创建，则返回null。
	 */
	public final V get(K key) {
		if (key == null) {
			throw new NullPointerException("key == null");
		}

		V mapValue;
		synchronized (this) {
			mapValue = map.get(key);
			if (mapValue != null) {
				hitCount++; // 命中
				return mapValue;
			}
			missCount++; // 丢失
		}

		/*
		 * Attempt to create a value. This may take a long time, and the map may
		 * be different when create() returns. If a conflicting value was added
		 * to the map while create() was working, we leave that value in the map
		 * and release the created value. 如果丢失了就试图创建一个item
		 */

		V createdValue = create(key);
		if (createdValue == null) {
			return null;
		}

		synchronized (this) {
			createCount++;// 创建++
			mapValue = map.put(key, createdValue);

			if (mapValue != null) {
				// There was a conflict so undo that last put
				// 如果前面存在oldValue，那么撤销put（）
				map.put(key, mapValue);
			} else {
				size += safeSizeOf(key, createdValue);
			}
		}

		if (mapValue != null) {
			entryRemoved(false, key, createdValue, mapValue);
			return mapValue;
		} else {
			trimToSize(maxSize);
			return createdValue;
		}
	}

	/**
	 * Caches {@code value} for {@code key}. The value is moved to the head of
	 * the queue.
	 * 
	 * @return the previous value mapped by {@code key}.
	 */
	public final V put(K key, V value) {
		if (key == null || value == null) {
			throw new NullPointerException("key == null || value == null");
		}

		V previous;
		synchronized (this) {
			putCount++;
			size += safeSizeOf(key, value);
			previous = map.put(key, value);
			if (previous != null) { // 返回的先前的value值
				size -= safeSizeOf(key, previous);
			}
		}

		if (previous != null) {
			entryRemoved(false, key, previous, value);
		}

		trimToSize(maxSize);
		return previous;
	}

	/**
	 * @param maxSize
	 *            the maximum size of the cache before returning. May be -1 to
	 *            evict even 0-sized elements. 清空cache空间
	 */
	private void trimToSize(int maxSize) {
		while (true) {
			K key;
			V value;
			synchronized (this) {
				if (size < 0 || (map.isEmpty() && size != 0)) {
					throw new IllegalStateException(getClass().getName()
							+ ".sizeOf() is reporting inconsistent results!");
				}

				if (size <= maxSize) {
					break;
				}

				// Map.Entry<K, V> toEvict = (Map.Entry<K, V>)map.eldest();

				Iterator<Entry<K, V>> iter = map.entrySet().iterator();
				Map.Entry<K, V> toEvict = null;
				while (iter.hasNext()) {

					toEvict = (Entry<K, V>) iter.next();
					break;
				}

				if (toEvict == null) {
					break;
				}

				key = toEvict.getKey();
				value = toEvict.getValue();
				map.remove(key);
				size -= safeSizeOf(key, value);
				evictionCount++;
			}

			entryRemoved(true, key, value, null);
		}
	}

	/**
	 * Removes the entry for {@code key} if it exists. 删除key相应的cache项，返回相应的value
	 * 
	 * @return the previous value mapped by {@code key}.
	 */
	public final V remove(K key) {
		if (key == null) {
			throw new NullPointerException("key == null");
		}

		V previous;
		synchronized (this) {
			previous = map.remove(key);
			if (previous != null) {
				size -= safeSizeOf(key, previous);
			}
		}

		if (previous != null) {
			entryRemoved(false, key, previous, null);
		}

		return previous;
	}

	/**
	 * Called for entries that have been evicted or removed. This method is
	 * invoked when a value is evicted to make space, removed by a call to
	 * {@link #remove}, or replaced by a call to {@link #put}. The default
	 * implementation does nothing.
	 * 当item被回收或者删掉时调用。改方法当value被回收释放存储空间时被remove调用， 或者替换item值时put调用，默认实现什么都没做。
	 * <p>
	 * The method is called without synchronization: other threads may access
	 * the cache while this method is executing.
	 * 
	 * @param evicted
	 *            true if the entry is being removed to make space, false if the
	 *            removal was caused by a {@link #put} or {@link #remove}.
	 *            true---为释放空间被删除；false---put或remove导致
	 * @param newValue
	 *            the new value for {@code key}, if it exists. If non-null, this
	 *            removal was caused by a {@link #put}. Otherwise it was caused
	 *            by an eviction or a {@link #remove}.
	 */
	protected void entryRemoved(boolean evicted, K key, V oldValue, V newValue) {
	}

	/**
	 * Called after a cache miss to compute a value for the corresponding key.
	 * Returns the computed value or null if no value can be computed. The
	 * default implementation returns null. 当某Item丢失时会调用到，返回计算的相应的value或者null
	 * <p>
	 * The method is called without synchronization: other threads may access
	 * the cache while this method is executing.
	 * 
	 * <p>
	 * If a value for {@code key} exists in the cache when this method returns,
	 * the created value will be released with {@link #entryRemoved} and
	 * discarded. This can occur when multiple threads request the same key at
	 * the same time (causing multiple values to be created), or when one thread
	 * calls {@link #put} while another is creating a value for the same key.
	 */
	protected V create(K key) {
		return null;
	}

	private int safeSizeOf(K key, V value) {
		int result = sizeOf(key, value);
		if (result < 0) {
			throw new IllegalStateException("Negative size: " + key + "="
					+ value);
		}
		return result;
	}

	/**
	 * Returns the size of the entry for {@code key} and {@code value} in
	 * user-defined units. The default implementation returns 1 so that size is
	 * the number of entries and max size is the maximum number of entries.
	 * 返回用户定义的item的大小，默认返回1代表item的数量，最大size就是最大item值
	 * <p>
	 * An entry's size must not change while it is in the cache.
	 */
	protected int sizeOf(K key, V value) {
		return 1;
	}

	/**
	 * Clear the cache, calling {@link #entryRemoved} on each removed entry.
	 * 清空cacke
	 */
	public final void evictAll() {
		trimToSize(-1); // -1 will evict 0-sized elements
	}

	/**
	 * For caches that do not override {@link #sizeOf}, this returns the number
	 * of entries in the cache. For all other caches, this returns the sum of
	 * the sizes of the entries in this cache.
	 */
	public synchronized final int size() {
		return size;
	}

	/**
	 * For caches that do not override {@link #sizeOf}, this returns the maximum
	 * number of entries in the cache. For all other caches, this returns the
	 * maximum sum of the sizes of the entries in this cache.
	 */
	public synchronized final int maxSize() {
		return maxSize;
	}

	/**
	 * Returns the number of times {@link #get} returned a value that was
	 * already present in the cache.
	 */
	public synchronized final int hitCount() {
		return hitCount;
	}

	/**
	 * Returns the number of times {@link #get} returned null or required a new
	 * value to be created.
	 */
	public synchronized final int missCount() {
		return missCount;
	}

	/**
	 * Returns the number of times {@link #create(Object)} returned a value.
	 */
	public synchronized final int createCount() {
		return createCount;
	}

	/**
	 * Returns the number of times {@link #put} was called.
	 */
	public synchronized final int putCount() {
		return putCount;
	}

	/**
	 * Returns the number of values that have been evicted. 返回被回收的数量
	 */
	public synchronized final int evictionCount() {
		return evictionCount;
	}

	/**
	 * Returns a copy of the current contents of the cache, ordered from least
	 * recently accessed to most recently accessed. 返回当前cache的副本，从最近最少访问到最多访问
	 */
	public synchronized final Map<K, V> snapshot() {
		return new LinkedHashMap<K, V>(map);
	}

	@Override
	public synchronized final String toString() {
		int accesses = hitCount + missCount;
		int hitPercent = accesses != 0 ? (100 * hitCount / accesses) : 0;
		return String.format(
				"LruCache[maxSize=%d,hits=%d,misses=%d,hitRate=%d%%]", maxSize,
				hitCount, missCount, hitPercent);
	}
}
