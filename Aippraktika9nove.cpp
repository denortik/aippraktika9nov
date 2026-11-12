#include <limits>
#include <cstddef>
#include <utility>
#include <stdexcept>

struct p_t {
    int x;
    int y;
};

class Ints {
public:
    Ints();
    Ints(size_t size, int value);
    Ints(Ints const& rhs);
    Ints(Ints&& rhs) noexcept;
    Ints(const int* arr, size_t arr_size);
    Ints(const size_t size);
    Ints& operator=(const Ints& rhs);
    Ints& operator=(Ints&& rhs) noexcept;
    Ints operator+(const Ints& rhs) const;
    ~Ints();

    size_t getSize() const noexcept;
    void set(size_t id, int i);
    int get(size_t id) const noexcept;

    int getSafe(size_t id) const;
    void setSafe(size_t id, int i);

    Ints append(int i) const;
    Ints append(const Ints& b) const;

private:
    size_t size;
    size_t capacity;
    int* data;
};

Ints::Ints() :
    size(0),
    capacity(0),
    data(nullptr)
{
}

Ints::Ints(size_t size, int value) :
    size(size), capacity(size), data(new int[capacity])
{
    for (size_t i = 0; i < size; i++) {
        data[i] = value;
    }
}

Ints::Ints(const int* arr, const size_t arr_size) :
    size(arr_size), capacity(size), data(new int[capacity])
{
    for (size_t i = 0; i < arr_size; i++) {
        data[i] = arr[i];
    }
}

Ints::Ints(const size_t size) :
    size(size), capacity(size), data(new int[capacity])
{
}

Ints::Ints(Ints const& rhs) :
    size(rhs.size), capacity(rhs.capacity), data(new int[size])
{
    for (size_t i = 0; i < size; i++) {
        data[i] = rhs.data[i];
    }
}

Ints::Ints(Ints&& rhs) noexcept :
    size(rhs.size),
    capacity(rhs.capacity),
    data(rhs.data)
{
    rhs.capacity = 0;
    rhs.size = 0;
    rhs.data = nullptr;
}

Ints& Ints::operator=(const Ints& rhs)
{
    if (this == &rhs) {
        return *this;
    }

    Ints temp(rhs);
    std::swap(size, temp.size);
    std::swap(data, temp.data);
    std::swap(capacity, temp.capacity);

    return *this;
}

Ints& Ints::operator=(Ints&& rhs) noexcept
{
    if (this != &rhs) {
        delete[] data;

        capacity = rhs.capacity;
        size = rhs.size;
        data = rhs.data;

        rhs.data = nullptr;
        rhs.capacity = 0;
        rhs.size = 0;
    }

    return *this;
}

Ints Ints::operator+(const Ints& rhs) const {
    if (getSize() != rhs.getSize()) {
        throw std::invalid_argument("Arrays have diff size");
    }

    Ints result(getSize(), 0);
    for (size_t i = 0; i < getSize(); i++) {
        int a = get(i);
        int b = rhs.get(i);
        if ((b > 0 && a > std::numeric_limits< int >::max() - b) || (b < 0 && a < std::numeric_limits< int >::min() - b)) {
            throw std::invalid_argument("Overflow");
        }
        result.set(i, b + a);
    }

    return result;
}

Ints::~Ints() {
    delete[] data;
}

void Ints::set(size_t id, int i) {
    data[id] = i;
}

void Ints::setSafe(size_t id, int i) {
    if (id >= size) {
        throw std::logic_error("Out of range");
    }

    set(id, i);
}

int Ints::get(size_t id) const noexcept {
    return data[id];
}

int Ints::getSafe(size_t id) const {
    if (id >= size) {
        throw std::logic_error("Out of range");
    }
    return get(id);
}

size_t Ints::getSize() const noexcept {
    return size;
}

Ints Ints::append(int i) const
{
    if (std::numeric_limits<size_t>::max() - 1 < size) {
        throw std::logic_error("Overflow");
    }

    Ints temp(size + 1, 0);

    for (size_t p = 0; p < size; ++p) {
        temp.data[p] = data[p];
    }
    temp.data[size] = i;

    return temp;
}

Ints Ints::append(const Ints& b) const
{
    Ints temp(*this);

    for (size_t i = 0; i < b.size; ++i) {
        temp = temp.append(b.data[i]);
    }

    return temp;
}

class Mtx {
    size_t m;
    size_t n;
    Ints data;

public:
    Mtx();
    Mtx(size_t m, size_t n);
    Mtx(size_t m, size_t n, const Ints& data);
    ~Mtx() = default;
    Mtx sum(Mtx const& rhs) const;
    Mtx mul(Mtx const& rhs) const;
    Ints getCol(size_t id) const;
    Ints getRow(size_t id) const;
    Mtx addRow(const int pasta);
    Mtx transpose() const;
    int det() const;
};

Mtx::Mtx() :
    m(0), n(0)
{
}

Mtx::Mtx(size_t m, size_t n) :
    m(m), n(n), data(n* m, 0)
{
}
Mtx::Mtx(size_t m, size_t n, const Ints& data) :
    m(m), n(n), data(data)
{
}

Mtx Mtx::sum(Mtx const& rhs) const {
    if (m != rhs.m || n != rhs.n) {
        throw std::invalid_argument("Matrices have diff size");
    }

    Mtx result(m, n);
    for (size_t i = 0; i < data.getSize(); i++) {
        int a = data.get(i);
        int b = rhs.data.get(i);
        if ((b > 0 && a > std::numeric_limits< int >::max() - b) || (b < 0 && a < std::numeric_limits< int >::min() - b)) {
            throw std::logic_error("Overflow");
        }
        result.data.set(i, b + a);
    }

    return result;
}

Ints Mtx::getCol(size_t id) const {
    Ints res;
    for (size_t i = 0; i < m; ++i) {
        res = res.append(data.get(i * n + id));
    }
    return res;
}

Ints Mtx::getRow(size_t id) const {
    Ints res;
    for (size_t j = 0; j < n; ++j) {
        res = res.append(data.get(id * n + j));
    }
    return res;
}

Mtx Mtx::mul(const Mtx& rhs) const {
    if (n != rhs.m) {
        throw std::invalid_argument("Matrices have incompatible sizes");
    }

    Mtx res(m, rhs.n);
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < rhs.n; j++) {
            Ints row = getRow(i);
            Ints col = rhs.getCol(j);

            int sm = 0;
            for (size_t k = 0; k < n; k++) {
                sm += row.get(k) * col.get(k);
            }

            res.data.set(i * rhs.n + j, sm);
        }
    }

    return res;
}

Mtx Mtx::addRow(const int pasta) {
    Mtx res(m + 1, n);
    for (size_t i = 0; i < m * n; i++) {
        res.data.set(i, data.get(i));
    }
    for (size_t i = 0; i < n; i++) {
        res.data.set(n * m + i, pasta);
    }
    return res;
}
Mtx Mtx::transpose() const {
    Mtx res(n, m);
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            res.data.set(j * m + i, data.get(i * n + j));
        }
    }
    return res;
}

int Mtx::det() const {
    if (m != n) {
        throw std::invalid_argument("Matrix must be square");
    }
    if (m == 0) {
        return 1;
    }
    if (m == 1) {
        return data.get(0);
    }
    if (m == 2) {
        return data.get(0) * data.get(3) - data.get(1) * data.get(2);
    }
    int res = 0;
    int sign = 1;
    for (size_t i = 0; i < n; i++) {
        Mtx minor(n - 1, n - 1);
        size_t idx = 0;
        for (size_t row = 1; row < m; row++) {
            for (size_t col = 0; col < n; col++) {
                if (col == i) continue;
                minor.data.set(idx++, data.get(row * n + col));
            }
        }
        res += sign * data.get(i) * minor.det();
        sign = -sign;
    }

    return res;
}

class Points {
    Ints data_points;
public:
    Points() :
        data_points(Ints())
    {
    }

    Points(const Ints& data) :
        data_points(data)
    {
    }

    ~Points() = default;

    void set(const size_t id, const p_t p) noexcept {
        data_points.set(id * 2, p.x);
        data_points.set(id * 2 + 1, p.y);
    }

    p_t get(const size_t id) const noexcept {
        p_t res;
        res.x = data_points.get(id * 2);
        res.y = data_points.get(id * 2 + 1);

        return res;
    }

    Points append(p_t p) const {
        size_t const size = data_points.getSize();

        if (size > std::numeric_limits<size_t>::max() - 2) {
            throw std::logic_error("Overflow");
        }

        Ints tmp(size + 2);

        for (size_t i = 0; i < size; i++) {
            tmp.set(i, data_points.get(i));
        }

        tmp.set(size, p.x);
        tmp.set(size + 1, p.y);

        return Points(tmp);
    }
};

class Plot
{
public:
    void set(const int x, const int y);
    int calc(int x) const;
    int solve(Ints& buf, int y) const;
    Points append(p_t p) const;
private:
    Ints data_plot;
};

void Plot::set(const int x, const int y) {
    for (size_t i = 0; i < data_plot.getSize(); i += 2) {
        if (data_plot.get(i) == x) {
            data_plot.set(i + 1, y);
            return;
        }
    }

    data_plot = data_plot.append(x);
    data_plot = data_plot.append(y);
}

int Plot::calc(const int x) const {
    for (size_t i = 0; i < data_plot.getSize(); i += 2) {
        if (data_plot.get(i) == x) {
            return data_plot.get(i + 1);
        }
    }

    throw std::logic_error("Point not found");
}

int Plot::solve(Ints& buf, const int y) const {
    int cnt = 0;
    for (size_t i = 1; i < data_plot.getSize(); i += 2) {
        if (y == data_plot.get(i)) {
            buf = buf.append(data_plot.get(i - 1));
            cnt++;
        }
    }
    return cnt;
}

Points Plot::append(const p_t p) const {
    size_t const size = data_plot.getSize();
    Ints tmp(size + 2);

    for (size_t i = 0; i < size; i++) {
        tmp.set(i, data_plot.get(i));
    }

    tmp.set(size, p.x);
    tmp.set(size + 1, p.y);

    return Points(tmp);
}
int main() {
    Ints a(10, 67);
    Ints b(10, 69);
    Ints c = a + b + a + b;
    return 0;
}