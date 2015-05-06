#include <memory>

#include <TTree.h>

#ifdef FROM_CMSSW
#include "../interface/TreeWrapper.h"
#else
#include <TreeWrapper.h>
#endif

namespace ROOT {
    TreeWrapper::TreeWrapper(TTree* tree):
        m_tree(tree),
        m_entry(-1) {
            init(tree);
        }

    TreeWrapper::TreeWrapper():
        m_tree(nullptr),
        m_entry(-1) {

        }

    void TreeWrapper::init(TTree* tree) {
        m_tree = tree;
        if (m_tree->GetListOfLeaves()->GetEntriesFast() > 0) {
            // Disable reading of all branches by default
            m_tree->SetBranchStatus("*", 0);
        }

        for (auto& leaf: m_leafs)
            leaf.second->init(m_tree);
    }

    /**
     * Read the next entry of the tree.
     * 
     * returns true in case of success, or false if the end of the tree is reached
     */
    bool TreeWrapper::next() {
        uint64_t entries = getEntries();

        if (m_entry + 1 > (int64_t) entries)
            return false;

        m_entry++;

        return m_tree->GetEntry(m_entry) > 0;
    }

    Leaf& TreeWrapper::operator[](const std::string& name) {

        if (m_leafs.count(name))
            return *m_leafs.at(name);

        std::shared_ptr<Leaf> leaf = std::make_shared<Leaf>(name, m_tree);
        m_leafs[name] = leaf;

        if (m_entry != -1) {
            // A global GetEntry already happened in the tree
            // Call GetEntry directly on the Branch to catch up
            leaf->getBranch()->GetEntry(m_entry);
        }

        return *leaf;
    }
};
