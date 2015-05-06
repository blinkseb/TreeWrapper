#pragma once

#include <memory>
#include <unordered_map>

#include "Leaf.h"

class TTree;

namespace ROOT {
    class TreeWrapper {

        public:
            TreeWrapper(TTree* tree);
            TreeWrapper();

            void init(TTree* tree);

            bool next();
            void rewind() {
                m_entry = -1;
            }

            void fill(bool reset = true) {
                m_tree->Fill();
                if (reset)
                    this->reset();
            }

            inline uint64_t getEntries() {
                return m_tree->GetEntries();
            }

            inline void reset() {
                for (auto& leaf: m_leafs)
                    leaf.second->reset();
            }

            Leaf& operator[](const std::string& name);

        private:
            TTree* m_tree;
            int64_t m_entry;

            std::unordered_map<std::string, std::shared_ptr<Leaf>> m_leafs;
    };
};
