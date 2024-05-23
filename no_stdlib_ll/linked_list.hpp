namespace my {

template <typename data_t>

class linked_list {
    template <typename data_t_> struct node_t {
        node_t<data_t_> *next;
        data_t_ data;

        node_t() = delete;
        node_t(const data_t_& dat, node_t<data_t_> *n = nullptr): next{n}, data{dat} {}
    };

public:
    class iterator {
        friend linked_list<data_t>;
        node_t<data_t> *current;
        node_t<data_t> *next;

    private:
        node_t<data_t>* swap(node_t<data_t> *node) noexcept {
            node_t<data_t> *buf = current;
            current->next = node;
            return buf;
        }

        explicit iterator(node_t<data_t> *nd): current{nd}, next{nd ? nd->next : nullptr} {}

    public:
        iterator(): current{nullptr}, next{nullptr} {}
        iterator(const iterator& it) noexcept : current{it.current}, next{it.next} {}

    public:
        data_t& data() noexcept {
            return current->data;
        }

        iterator& operator++ () noexcept {
            if (next == nullptr) {
                current = nullptr;
                return *this;
            }
            node_t<data_t> *buf = next->next;
            current = next;
            next = buf;
        
            return *this;
        }

        iterator operator ++(int) noexcept {
            iterator old{*this};
            ++(*this);
            return old;
        }

        bool operator == (const iterator& a) const noexcept {
            return (current == a.current) and (next == a.next);
        }

        bool operator != (const iterator& a) const noexcept {
            return !((*this) == a);
        }
    };


private:
    node_t<data_t> *start;
    int size;
    iterator front_;
    const iterator back_{};

    void pop_top() noexcept {
        node_t<data_t> *buf = start;
        start = buf->next;
        delete buf;
        front_ = iterator{start};
        --size;
    }

public:
    linked_list(): start{nullptr}, size{0} {}

    explicit linked_list(const data_t& data): start{new node_t<data_t>{data}}, size{1}, front_{start} {}

    linked_list(const linked_list& list) = delete;

    linked_list(linked_list&& list) noexcept {
        start = list.start;
        list.start = nullptr;
        front_ = list.front_;
        list.front_ = back_;
        size = list.size;
        list.size = 0;
    }

    linked_list& operator = (const linked_list& list) = delete;

    linked_list& operator = (linked_list&& list) {
        clean();
        start = list.start;
        size = list.size;
        front_ = list.front_;
        list.start = nullptr;
        list.front_ = back_;
        list.size = 0;
    }

    ~linked_list() {
        clean();
    }


public:
    void push(const data_t& data) {
        auto new_start = new node_t<data_t>{data, start};
        start = new_start;
        front_ = iterator{start};
        ++size;
    }

    data_t pop() noexcept(noexcept(data_t::data_t(&))) {
        data_t buf_d = start->data;
        pop_top();
        return buf_d;
    }

    void clean() noexcept {
        while (size) this->pop_top();
    }

    bool is_empty() const noexcept {
        return !size;
    }

    void reverse() noexcept {
        node_t<data_t> *buf = nullptr;
        for (auto i{front_}; i != back_; ++i) buf = i.swap(buf);
        start = buf;
        front_.current = start;
        front_.next = start->next;
    }


public:
    const iterator& begin() const & {
        return front_;
    }
    
    const iterator& back() const &  {
        return back_;
    }
};

}