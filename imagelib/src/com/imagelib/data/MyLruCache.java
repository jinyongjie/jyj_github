package com.imagelib.data;

import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Map.Entry;

/**
 * A cache that holds strong references to a limited number of values. Each time
 * a value is accessed, it is moved to the head of a queue. When a value is
 * added to a full cache, the value at the end of that queue is evicted and may
 * become eligible for garbage collection.
 * Cache����һ��ǿ��������������������ÿ��Item�����ʵ�ʱ�򣬴�Item�ͻ��ƶ������е�ͷ����
 * ��cache�����ʱ������µ�itemʱ���ڶ���β����item�ᱻ���ա�
 * <p>
 * If your cached values hold resources that need to be explicitly released,
 * override {@link #entryRemoved}. �����cache��ĳ��ֵ��Ҫ��ȷ�ͷţ���дentryRemoved()
 * <p>
 * If a cache miss should be computed on demand for the corresponding keys,
 * override {@link #create}. This simplifies the calling code, allowing it to
 * assume a value will always be returned, even when there's a cache miss.
 * ���key���Ӧ��item����������дcreate().����˵��ô��룬��ʹ��ʧ��Ҳ�ܻ᷵�ء�
 * <p>
 * By default, the cache size is measured in the number of entries. Override
 * {@link #sizeOf} to size the cache in different units. For example, this cache
 * is limited to 4MiB of bitmaps:
 * Ĭ��cache��С�ǲ�����item����������дsizeof���㲻ͬitem�� ��С��
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
 * the key was not in the cache. ������key����valueΪnull
 * ��get������put������remove��������ֵΪnullʱ��key��Ӧ�����cache��
 */
public class MyLruCache<K, V> {
	private final LinkedHashMap<K, V> map;

	/** Size of this cache in units. Not necessarily the number of elements. */
	private int size; // �Ѿ��洢�Ĵ�С
	private int maxSize; // �涨�����洢�ռ�

	private int putCount; // put�Ĵ���
	private int createCount; // create�Ĵ���
	private int evictionCount; // ���յĴ���
	private int hitCount; // ���еĴ���
	private int missCount; // ��ʧ�Ĵ���

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
	 * be created.
	 * ͨ��key������Ӧ��item�����ߴ���������Ӧ��item����Ӧ��item���ƶ������е�ͷ����
	 * ���item��valueû�б�cache���߲��ܱ��������򷵻�null��
	 */
	public final V get(K key) {
		if (key == null) {
			throw new NullPointerException("key == null");
		}

		V mapValue;
		synchronized (this) {
			mapValue = map.get(key);
			if (mapValue != null) {
				hitCount++; // ����
				return mapValue;
			}
			missCount++; // ��ʧ
		}

		/*
		 * Attempt to create a value. This may take a long time, and the map may
		 * be different when create() returns. If a conflicting value was added
		 * to the map while create() was working, we leave that value in the map
		 * and release the created value. ���ʧ�˾���ͼ����һ��item
		 */

		V createdValue = create(key);
		if (createdValue == null) {
			return null;
		}

		synchronized (this) {
			createCount++;// ����++
			mapValue = map.put(key, createdValue);

			if (mapValue != null) {
				// There was a conflict so undo that last put
				// ���ǰ�����oldValue����ô����put����
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
			if (previous != null) { // ���ص���ǰ��valueֵ
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
	 *            evict even 0-sized elements. ���cache�ռ�
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
	 * Removes the entry for {@code key} if it exists.
	 * ɾ��key��Ӧ��cache�������Ӧ��value
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
	 * ��item�����ջ���ɾ��ʱ���á��ķ�����value�������ͷŴ洢�ռ�ʱ��remove���ã�
	 * �����滻itemֵʱput���ã�Ĭ��ʵ��ʲô��û����
	 * <p>
	 * The method is called without synchronization: other threads may access
	 * the cache while this method is executing.
	 * 
	 * @param evicted
	 *            true if the entry is being removed to make space, false if the
	 *            removal was caused by a {@link #put} or {@link #remove}.
	 *            true---Ϊ�ͷſռ䱻ɾ��false---put��remove����
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
	 * default implementation returns null.
	 * ��ĳItem��ʧʱ����õ������ؼ������Ӧ��value����null
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
	 * �����û������item�Ĵ�С��Ĭ�Ϸ���1���item�����������size�������itemֵ
	 * <p>
	 * An entry's size must not change while it is in the cache.
	 */
	protected int sizeOf(K key, V value) {
		return 1;
	}

	/**
	 * Clear the cache, calling {@link #entryRemoved} on each removed entry.
	 * ���cacke
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
	 * Returns the number of values that have been evicted. ���ر����յ�����
	 */
	public synchronized final int evictionCount() {
		return evictionCount;
	}

	/**
	 * Returns a copy of the current contents of the cache, ordered from least
	 * recently accessed to most recently accessed.
	 * ���ص�ǰcache�ĸ�������������ٷ��ʵ�������
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
