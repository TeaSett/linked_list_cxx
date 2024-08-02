#include <memory>
#include <utility>
#include <iterator>


namespace my {


template <typename data_t>

class linked_list {

    struct node;
    using p_node = std::shared_ptr<node>;
    struct node {

        p_node next;
        data_t data;

        node() = delete;
        node(const node&) = delete;
        node(node&& nd) : next{nd.next}, data{std::move(nd.data)} {}
        node(data_t data_, const p_node& nd) : next{nd}, data{data_} {}
        ~node() {}
    };


private://///////////////////////////        FIELDS        ////////////////////////////////////

    p_node top;
    std::size_t size_;


public://////////////////////////////     CONSTRUCTORS     ////////////////////////////////////
    
    linked_list() noexcept : top{nullptr}, size_{0} {}
    linked_list(linked_list&& list) : top{list.top}, size_{list.size_} { list.size_ = 0; }

    linked_list& operator = (linked_list&& list) {
        clean();
        std::swap(*this, list);
        return *this;
    }

    linked_list(const linked_list&) = delete;
    linked_list& operator = (const linked_list& list) = delete;
    ~linked_list() {}


public://////////////////////////////        METHODS         ////////////////////////////////////

    void push(data_t data) noexcept(noexcept(std::is_nothrow_copy_constructible_v<data_t> && std::is_nothrow_move_constructible_v<data_t>)) {
        p_node new_node = std::make_shared<node>(data, top);
        std::swap(top, new_node);
        ++size_;
    }

    data_t pop() noexcept {
        auto buf{top};
        std::swap(top, buf->next);
        --size_;
        return buf->data;
    }

    void clean() noexcept {
        while (size_) pop();
    }

    std::size_t size() const noexcept {
        return size_;
    }

    void reverse() {
        p_node buf = nullptr;
        for (auto i{ibegin()}; i != iend(); ++i) buf = i.swap(buf);
        top = buf;
    }


private://////////////////////////////   ITERATOR INTERFACE   ////////////////////////////////////

    using nodes_pair = std::pair<p_node, p_node>;
    template <class It, class data_type> class Iiterator {
    protected:
        friend class linked_list;
        nodes_pair nodes;

    public:
        using value_type = data_type;
        using difference_type = ptrdiff_t;
        using pointer = data_type*;
        using reference = data_type&;

    protected:
        explicit Iiterator(nodes_pair pair) noexcept : nodes{pair} {}
        Iiterator(const p_node& first, const p_node& second) noexcept : nodes{first, second} {}
    public:
        Iiterator() noexcept : nodes{nullptr, nullptr} {}
        Iiterator(const Iiterator& i) noexcept : nodes{i.nodes} {}
        It& operator = (const It& i) noexcept {
            nodes = i.nodes;
            return *this;
        }

        pointer operator -> () const {
            return &(nodes.first->data);
        }

        virtual reference operator * () const {
            return nodes.first->data;
        }

        It& operator ++() {
            if (nodes.second == nullptr) {
                nodes = {nullptr, nullptr};
                return *static_cast<It*>(this);
            }
            nodes.first = nodes.second;
            nodes.second = nodes.second->next;
            return *static_cast<It*>(this);
        }

        It& operator++ (int) {
            auto buf = static_cast<It*>(this);
            operator++();
            return *buf;
        }

        bool operator == (const It& i) { return !(operator!=(i)); }
        bool operator != (const It& i) { return (nodes.first != i.nodes.first) or (nodes.second != i.nodes.second); }

    };


    template <class It> using Iiterator_t = Iiterator<It, data_t>;
    template <class It> using const_Iiterator_t = Iiterator<It, const data_t>;


public://////////////////////////////       ITERATORS        ////////////////////////////////////

    class iterator final : public Iiterator_t<iterator>, public std::forward_iterator_tag {
        friend class linked_list;
        using itype = Iiterator_t<iterator>;

    protected:
        explicit iterator(nodes_pair pair) noexcept : Iiterator_t<iterator>(pair) {}
        iterator(const p_node& first, const p_node& second) noexcept : Iiterator_t<iterator>(first, second) {}
    public:
        iterator() noexcept : Iiterator_t<iterator>{} {}
        iterator(const iterator& i) noexcept : Iiterator_t<iterator>{i} {}
    };


    class const_iterator final : public const_Iiterator_t<const_iterator>, public std::forward_iterator_tag {
        friend class linked_list;
        using itype = const_Iiterator_t<const_iterator>;

    protected:
        explicit const_iterator(nodes_pair pair) noexcept : const_Iiterator_t<const_iterator>(pair) {}
        const_iterator(const p_node& first, const p_node& second) noexcept : const_Iiterator_t<const_iterator>(first, second) {}
    public:
        const_iterator() noexcept : const_Iiterator_t<const_iterator>{} {}
        const_iterator(const const_iterator& i) noexcept : const_Iiterator_t<const_iterator>{i} {}
    };


private:

    class inverse_iterator final : public Iiterator_t<inverse_iterator>, public std::forward_iterator_tag {
        friend class linked_list;
        using itype = Iiterator_t<inverse_iterator>;

        p_node swap(const p_node& nd) {
            auto buf = itype::nodes.first;
            itype::nodes.first->next = nd;
            return buf;
        }

    protected:
        explicit inverse_iterator(nodes_pair pair) noexcept : Iiterator_t<inverse_iterator>(pair) {}
        inverse_iterator(const p_node& first, const p_node& second) noexcept : Iiterator_t<inverse_iterator>(first, second) {}
    public:
        inverse_iterator() noexcept : Iiterator_t<inverse_iterator>{} {}
        inverse_iterator(const inverse_iterator& i) noexcept : Iiterator_t<inverse_iterator>{i} {}
    };


private:
    const iterator end_{};
    const const_iterator cend_{};
    const inverse_iterator iend_{};

private:
    inverse_iterator ibegin() const noexcept { return {top, top->next}; }
    inverse_iterator iend() const noexcept { return iend_; }
public:
    iterator begin() const noexcept { return {top, top->next}; }
    iterator end() const noexcept { return end_; }
    const_iterator cbegin() const noexcept { return {top, top->next}; }
    const_iterator cend() const noexcept { return cend_; }
    
};








} // my namespace