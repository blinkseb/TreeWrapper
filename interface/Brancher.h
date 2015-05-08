#pragma once

#include <string>

#include <TTree.h>

struct Brancher {
    public:
        virtual void operator()(const std::string&, TTree* tree) = 0;
};

template <typename T>
struct BranchCreaterT: Brancher {
    public:
        BranchCreaterT(T& data)
            : m_data(data) {
            }

        virtual void operator()(const std::string& name, TTree* tree) {
            tree->Branch<T>(name.c_str(), &m_data);
        }

    private:
        T& m_data;
};

template <typename T>
struct BranchReaderT: Brancher {
    public:
        BranchReaderT(T* data, TBranch** branch)
            : m_data(data), m_branch(branch) {
            }

        virtual void operator()(const std::string& name, TTree* tree) {
            tree->SetBranchAddress<T>(name.c_str(), m_data, m_branch);
            (*m_branch)->SetStatus(1);
        }

    private:
        T* m_data;
        TBranch** m_branch;
};
