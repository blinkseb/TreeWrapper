#pragma once

struct Resetter {
    public:
        virtual void reset() = 0;
};

template <typename T>
struct ResetterT: Resetter {
    public:
        ResetterT(T& data)
            : m_data(data) {
            }

        virtual void reset() {
            // By default, set data to 0
            // This is not suitable for every type, so specialized templates are provided for other types
            m_data = 0;
        }

    private:
        T& m_data;
};

template <typename T>
struct ResetterT<std::vector<T>>: Resetter {
    public:
        ResetterT(std::vector<T>& data)
            : m_data(data) {
            }

        virtual void reset() {
            m_data.clear();
        }

    private:
        std::vector<T>& m_data;
};
