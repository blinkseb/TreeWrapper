#pragma once

#include <boost/any.hpp>
#include <memory>

#include <TTree.h>

#include "Brancher.h"
#include "Resetter.h"

namespace ROOT {

    class TreeWrapper;

    /* This class holds anything related to the branch
     *
     * The only way to create a Leaf instance is from TreeWrapper <TreeWrapper::operator[]>.
     */
    class Leaf {
        private:
            Leaf(const std::string& name, TTree* tree);

        public:
            /* Register this branch for write access
             * @T Type of data this branch holds
             *
             * Register this branch for write access. A new branch will be created in the tree properly configured to hold data of type T.
             *
             * Internally, the `TTree::Branch` method is called to create the new branch.
             *
             * @return a reference to the data hold by this branch. Change the content of this reference before calling <TreeWrapper::fill> to change the branch data.
             */
            template<typename T> T& write() {
                if (m_data.empty()) {
                    // Initialize boost::any with empty data.
                    // This allocate the necessary memory
                    m_data = boost::any(T());

                    T& data = boost::any_cast<T&>(m_data);
                    m_resetter.reset(new ResetterT<T>(data));

                    if (m_tree) {
                        // Register this Leaf in the tree
                        m_tree->Branch<T>(m_name.c_str(), &data);
                    } else {
                        m_brancher.reset(new BranchCreaterT<T>(data));
                    }
                }

                return boost::any_cast<T&>(m_data);
            };

            /* Register this branch for read access
             * @T Type of data this branch holds
             *
             * Register this branch for read access. The branch must exists in the tree and created to hold data of type T.
             *
             * Internally, the `TTree::SetBranchAddress` method is called to read the branch. The status of the branch is also set to 1.
             *
             * @return a const reference to the data hold by this branch. The content is in read-only mode, and will change each time <TreeWrapper::next> is called.
             */
            template<typename T> const T& read() {
                if (m_data.empty()) {
                    // Initialize boost::any with empty data.
                    // This allocate the necessary memory
                    m_data = boost::any(std::shared_ptr<T>(new T()));

                    T* data = boost::any_cast<std::shared_ptr<T>>(m_data).get();
                    m_resetter.reset(new ResetterT<T>(*data));

                    if (m_tree) {
                        m_tree->SetBranchAddress<T>(m_name.c_str(), data, &m_branch);
                        // Enable read for this branch
                        ROOT::utils::activateBranch(m_branch);
                    } else {
                        m_brancher.reset(new BranchReaderT<T>(data, &m_branch));
                    }
                }

                // Return a const since we read from the tree
                return const_cast<const T&>(*boost::any_cast<std::shared_ptr<T>>(m_data));
            }

        private:
            void init(TTree* tree) {
                m_tree = tree;
                if (m_brancher.get())
                    (*m_brancher)(m_name, m_tree);
            }

            void reset() {
              if (m_resetter.get()) {
                  m_resetter->reset();
              }
            }

            TBranch* getBranch() {
                return m_branch;
            }

            Leaf(const Leaf&) = delete;
            Leaf& operator=(const Leaf&) = delete;

        private:

            friend class TreeWrapper;

            boost::any m_data;
            TBranch* m_branch;

            std::string m_name;
            TTree* m_tree;

            std::unique_ptr<Resetter> m_resetter;
            std::unique_ptr<Brancher> m_brancher;
    };
};
