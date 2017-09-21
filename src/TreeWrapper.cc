#include <memory>

#include <TChain.h>
#include <TTree.h>

#ifdef FROM_CMSSW
#include "../interface/TreeWrapper.h"
#else
#include <TreeWrapper.h>
#endif

namespace ROOT {
    TreeWrapper::TreeWrapper(TTree* tree):
        m_tree(tree),
        m_chain(nullptr),
        m_entry(0) {
            init(tree);
        }

    TreeWrapper::TreeWrapper():
        m_tree(nullptr),
        m_chain(nullptr),
        m_entry(0) {

        }

    TreeWrapper::TreeWrapper(const TreeWrapper& o) {
        m_tree = o.m_tree;
        m_chain = o.m_chain;
        m_leafs = o.m_leafs;
        m_varrGroups = o.m_varrGroups;
    }

    TreeWrapper::TreeWrapper(TreeWrapper&& o) {
        m_tree = o.m_tree;
        m_chain = o.m_chain;
        m_leafs = std::move(o.m_leafs);
        m_varrGroups = std::move(o.m_varrGroups);
    }

    void TreeWrapper::init(TTree* tree) {
        m_tree = tree;
        m_chain = dynamic_cast<TChain*>(tree);
        if (m_chain)
            m_chain->LoadTree(0);

        for (auto& leaf: m_leafs)
            leaf.second->init(this);
        for (auto& vGroup : m_varrGroups)
            vGroup.second->init(this);
    }

    /**
     * Read the next entry of the tree.
     * 
     * returns true in case of success, or false if the end of the tree is reached
     */
    bool TreeWrapper::next(bool readall/* = false*/) {
        uint64_t stop_at = getStopAt();

        if (m_entry >= stop_at)
            return false;

        bool result = getEntry(m_entry, readall);
        m_entry++;

        return result;
    }

    bool TreeWrapper::getEntry(uint64_t entry, bool readall/* = false*/) {

        if (! m_cleaned) {
            for (auto it = m_leafs.begin(); it != m_leafs.end(); ) {
                if (it->second->getBranch() == nullptr) {
                    it = m_leafs.erase(it);
                } else
                    ++it;
            }
            for (auto& vGroup : m_varrGroups) {
              for (auto it = vGroup.second->m_leafs.begin(); vGroup.second->m_leafs.end() != it; ++it) {
                if ( ! it->second->getBranch() ) {
                  it = vGroup.second->m_leafs.erase(it);
                } else {
                  ++it;
                }
              }
            }

            m_cleaned = true;
        }

        if (readall) {
            if (! m_tree->GetEntry(entry, 1))
                return false;
        } else {
            uint64_t local_entry = entry;
            if (m_chain) {
                int64_t tree_index = m_chain->LoadTree(local_entry);
                if (tree_index < 0) {
                    std::cerr << "ERROR: LoadTree failed. Return code: " << tree_index << std::endl;
                    return false;
                }

                local_entry = static_cast<uint64_t>(tree_index);
            }

            for (auto& leaf: m_leafs) {
                int res = leaf.second->getBranch()->GetEntry(local_entry);
                if (res <= 0) {
                    std::cerr << "ERROR: GetEntry failed for branch " << leaf.first << ". Return code: " << res << std::endl;
                    return false;
                }
            }
        }
        for ( auto& vGroup : m_varrGroups ) {
          vGroup.second->getEntry(entry, readall);
        }

        m_entry = entry;
        return true;
    }

    void TreeWrapper::setEntry(uint64_t entry) {
        uint64_t stop_at = getStopAt();

        if (entry >= stop_at)
            m_entry = stop_at - 1;
        else
            m_entry = entry;
    }

    void TreeWrapper::stopAt(uint64_t entry) {
        m_stop_at_set = true;
        if (entry >= getEntries())
            m_stop_at = getEntries();
        else
            m_stop_at = entry + 1;
    }

    Leaf& TreeWrapper::operator[](const std::string& name) {

        if (m_leafs.count(name))
            return *m_leafs.at(name);

        std::shared_ptr<Leaf> leaf(new Leaf(name, this));
        m_leafs[name] = leaf;

        return *leaf;
    }

    TreeGroup TreeWrapper::group(const std::string& prefix) {
        return TreeGroup(prefix, *this);
    }
};
