#pragma once

#include <boost/any.hpp>
#include <memory>

#include <TTree.h>

namespace ROOT {
    class Leaf {
        public:
            Leaf(const std::string& name, TTree* tree);

            template<typename T> T& write() {
                if (m_data.empty()) {
                    // Initialize boost::any with empty data.
                    // This allocate the necessary memory
                    m_data = boost::any(T());

                    T& data = boost::any_cast<T&>(m_data);

                    // Register this Leaf in the tree
                    m_tree->Branch<T>(m_name.c_str(), &data);
                }

                return boost::any_cast<T&>(m_data);
            };

            template<typename T> const T& read() {
                if (m_data.empty()) {
                    // Initialize boost::any with empty data.
                    // This allocate the necessary memory
                    m_data = boost::any(std::shared_ptr<T>(new T()));

                    T* data = boost::any_cast<std::shared_ptr<T>>(m_data).get();
                    m_tree->SetBranchAddress<T>(m_name.c_str(), data, &m_branch);

                    // Enable read for this branch
                    m_branch->SetStatus(1);
                }

                // Return a const since we read from the tree
                return const_cast<const T&>(*boost::any_cast<std::shared_ptr<T>>(m_data));
            }


            Leaf(const Leaf&) = delete;
            Leaf& operator=(const Leaf&) = delete;

        private:

            friend class TreeWrapper;

            TBranch* getBranch() {
                return m_branch;
            }

            boost::any m_data;
            TBranch* m_branch;

            std::string m_name;
            TTree* m_tree;

    };
};
