
#include "../interface/TreeGroup.h"
#include "../interface/TreeWrapper.h"

namespace ROOT {
    Leaf& TreeGroup::operator[](const std::string& name) {
        return m_wrapper.operator[](m_prefix + name);
    }

    TreeGroup TreeGroup::group(const std::string& prefix) const {
        return TreeGroup(prefix + this->m_prefix, this->m_wrapper);
    }
}
