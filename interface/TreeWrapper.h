#pragma once

#include <memory>
#include <unordered_map>

#include "Leaf.h"

class TTree;

namespace ROOT {
    class TreeWrapper {

        public:
            TreeWrapper(TTree* tree);

            bool next();
            void rewind() {
                m_entry = -1;
            }

            void fill() {
                m_tree->Fill();
            }

            inline uint64_t getEntries() {
                return m_tree->GetEntries();
            }

            Leaf& operator[](const std::string& name);

        private:
            TTree* m_tree;
            int64_t m_entry;

            std::unordered_map<std::string, std::shared_ptr<Leaf>> m_leafs;
    };
};
