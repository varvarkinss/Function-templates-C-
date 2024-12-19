#pragma once
#include <list>
#include <vector>

#define ITERATOR_IMPLEMENTED

template <class T>
class UnorderedSet {
  std::list<T> elements_;
  std::vector<typename std::list<T>::const_iterator> buckets_;
  size_t NewBucketCount() const {
    return buckets_.empty() ? 1 : buckets_.size() * 2;
  }
  void PreInsert(const T& key) {
    if (Find(key)) {
      return;
    }
    if (elements_.size() >= buckets_.size()) {
      size_t new_bucket_count = NewBucketCount();
      Rehash(new_bucket_count);
    }
  }

 public:
  using Iterator = typename std::list<T>::const_iterator;
  using ConstIterator = typename std::list<T>::const_iterator;
  using DifferenceType = typename std::list<T>::difference_type;

  UnorderedSet() = default;
  UnorderedSet(size_t count) {  // NOLINT
    for (size_t i = 0; i < count; ++i) {
      buckets_.push_back(Iterator{});
    }
  }
  template <class Iter, class = std::enable_if_t<std::is_base_of_v<
                            std::forward_iterator_tag, typename std::iterator_traits<Iter>::iterator_category>>>
  UnorderedSet(Iter begin, Iter end) {
    size_t buckets = 0;
    for (auto it = begin; it != end; ++it) {
      buckets_.push_back(Iterator{});
      ++buckets;
    }
    for (; begin != end; ++begin) {
      std::hash<T> hash{};
      size_t bucket = hash(*begin) % buckets_.size();
      if (buckets_[bucket] == Iterator{}) {
        elements_.push_front(*begin);
        buckets_[bucket] = elements_.begin();
      } else {
        elements_.insert(buckets_[bucket], *begin);
        --buckets_[bucket];
      }
    }
  }
  float LoadFactor() const {
    if (buckets_.empty()) {
      return 0.0f;
    }
    return 1.0f * elements_.size() / buckets_.size();
  }

  Iterator begin() {  //  NOLINT
    return elements_.begin();
  }
  Iterator end() {  //  NOLINT
    return elements_.end();
  }

  ConstIterator begin() const {  //  NOLINT
    return begin();
  }
  ConstIterator end() const {  //  NOLINT
    return end();
  }

  ConstIterator cbegin() const {  //  NOLINT
    return begin();
  }

  ConstIterator cend() const {  //  NOLINT
    return end();
  }

  UnorderedSet(const UnorderedSet& other) : elements_(other.elements_), buckets_(other.buckets_) {
  }
  UnorderedSet(UnorderedSet&& other) noexcept
      : elements_(std::move(other.elements_)), buckets_(std::move(other.buckets_)) {
  }

  void Swap(UnorderedSet& other) {
    std::swap(elements_, other.elements_);
    std::swap(buckets_, other.buckets_);
  }
  UnorderedSet& operator=(const UnorderedSet& other) {  // todo переделать
    if (this != &other) {
      UnorderedSet(other).Swap(*this);
    }
    return *this;
  }

  UnorderedSet& operator=(UnorderedSet&& other) noexcept {
    if (this != &other) {
      UnorderedSet(std::move(other)).Swap(*this);
    }
    return *this;
  }

  size_t Size() const {
    return elements_.size();
  }
  bool Empty() const {
    return elements_.empty();
  }
  void Clear() {
    elements_.clear();
  }
  size_t Bucket(const T& key) const {
    std::hash<T> hash{};
    return hash(key) % buckets_.size();
  }

  void Insert(const T& key) {
    PreInsert(key);
    std::hash<T> hash{};
    size_t bucket = hash(key) % buckets_.size();
    if (buckets_[bucket] == Iterator{}) {
      elements_.emplace_front(key);
      buckets_[bucket] = elements_.begin();
    } else {
      elements_.insert(buckets_[bucket], key);
      buckets_[bucket]--;
    }
  }

  void Insert(T&& key) noexcept {
    PreInsert(key);
    std::hash<T> hash{};
    size_t bucket = hash(key) % buckets_.size();
    if (buckets_[bucket] == Iterator{}) {
      elements_.emplace_front(key);
      buckets_[bucket] = elements_.begin();
    } else {
      elements_.insert(buckets_[bucket], std::move(key));
      buckets_[bucket]--;
    }
  }

  void Erase(const T& key) {
    if (Empty()) {
      return;
    }
    std::hash<T> hash{};
    size_t bucket = hash(key) % buckets_.size();
    auto next = std::next(buckets_[bucket]);
    auto iter = buckets_[bucket];
    if (iter != Iterator{}) {
      size_t size = BucketSize(bucket);
      for (size_t i = 0; i < size; ++i, ++iter) {
        if (*iter == key) {
          if (iter == buckets_[bucket]) {
            buckets_[bucket] = next;
            if (size == 1) {
              buckets_[bucket] = Iterator{};
            }
          }
          elements_.erase(iter);
          return;
        }
      }
    }
  }

  bool Find(const T& key) const {
    if (Empty()) {
      return false;
    }
    bool f = false;
    std::hash<T> hash{};
    size_t bucket = hash(key) % buckets_.size();
    auto iter = buckets_[bucket];
    if (iter != Iterator{}) {
      size_t size = BucketSize(bucket);
      for (size_t i = 0; i < size; ++iter, ++i) {
        if (*iter == key) {
          f = true;
          break;
        }
      }
    }
    return f;
  }
  void Reserve(size_t new_bucket_count) {
    if (new_bucket_count >= elements_.size() && new_bucket_count > buckets_.size()) {
      std::list<T> elements_temp;
      std::vector<Iterator> buckets_temp;
      for (size_t i = 0; i < new_bucket_count; ++i) {
        buckets_temp.push_back(Iterator{});
      }
      if (Empty()) {
        elements_ = std::move(elements_temp);
        buckets_ = std::move(buckets_temp);
        return;
      }
      std::hash<T> hash{};
      auto it = elements_.begin();
      for (auto elem : elements_) {
        size_t bucket = hash(*it) % new_bucket_count;
        auto next = ++it;
        --it;
        if (buckets_temp[bucket] == Iterator{}) {
          elements_temp.insert(elements_temp.begin(), elem);
          buckets_temp[bucket] = elements_temp.begin();
        } else {
          elements_temp.insert(buckets_temp[bucket], elem);
          --buckets_temp[bucket];
        }
        it = next;
      }
      elements_ = std::move(elements_temp);
      buckets_ = std::move(buckets_temp);
    }
  }
  void Rehash(size_t new_bucket_count) {
    if (new_bucket_count >= elements_.size() && new_bucket_count != buckets_.size()) {
      std::list<T> elements_temp;
      std::vector<Iterator> buckets_temp;
      for (size_t i = 0; i < new_bucket_count; ++i) {
        buckets_temp.push_back(Iterator{});
      }
      if (Empty()) {
        elements_ = std::move(elements_temp);
        buckets_ = std::move(buckets_temp);
        return;
      }
      std::hash<T> hash{};
      auto it = elements_.begin();
      for (auto elem : elements_) {
        size_t bucket = hash(*it) % new_bucket_count;
        auto next = ++it;
        --it;
        if (buckets_temp[bucket] == Iterator{}) {
          elements_temp.insert(elements_temp.begin(), elem);
          buckets_temp[bucket] = elements_temp.begin();
        } else {
          elements_temp.insert(buckets_temp[bucket], elem);
          --buckets_temp[bucket];
        }
        it = next;
      }
      elements_ = std::move(elements_temp);
      buckets_ = std::move(buckets_temp);
    }
  }
  size_t BucketCount() const {
    return buckets_.size();
  }

  size_t BucketSize(size_t id) const {
    if (Empty()) {
      return 0u;
    }
    if (buckets_.size() <= id) {
      return 0u;
    }
    std::hash<T> hash{};
    size_t b_size = 0;
    for (Iterator it = buckets_[id]; *it != elements_.back() && (hash(*it) % buckets_.size()) == id; ++it) {
      ++b_size;
    }
    if ((hash(elements_.back()) % buckets_.size()) == id) {
      ++b_size;
    }
    return b_size;
  }
};