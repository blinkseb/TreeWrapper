#pragma once

#include "TreeWrapperAccessor.h"
#include "Brancher.h"

namespace ROOT {
  class VarrGroup;

  /* This class holds anything related to a variable-sized array branch
   *
   * The only ways to create a VarrLeaf are from VarrGroup <VarrGroup::operator[]> and TreeWrapper <TreeWrapper::var>
   */
  class VarrLeaf {
    private:
      VarrLeaf(std::string name, std::string lengthName, const TreeWrapperAccessor& tree)
        : m_name(name)
        , m_lengthLeafName(lengthName)
        , m_tree(tree)
      {}
    public:
      VarrLeaf(const VarrLeaf&) = delete;
      VarrLeaf(VarrLeaf&&) = delete;
      VarrLeaf& operator=(const VarrLeaf&) = delete;
      VarrLeaf& operator=(VarrLeaf&&) = delete;

      /* Name accessor
       * @return the name of the leaf
       */
      const std::string& name() const { return m_name; }

      /* Register this branch for read access
       * @maxsize capacity of the read buffer
       * @T Type of data this branch holds
       *
       * Register this branch for read access. The branch must exists in the tree and created to hold data of type T.
       *
       * Internally, the `TTree::SetBranchAddress` method is called to read the branch. The status of the branch is also set to 1.
       *
       * @return a const reference to the data hold by this branch. The content is in read-only mode, and will change each time <TreeWrapper::next> is called.
       */
      template<typename T> const std::vector<T>& read(std::size_t maxsize=100)
      {
        using data_type = std::vector<T>;
        if ( m_data.empty() && ( ! m_data_ptr ) ) {
          if ( m_tree.tree() ) {
            m_branch = m_tree.tree()->GetBranch(m_name.c_str());
            if ( ! m_branch ) {
              std::cout << "Warning: branch '" << m_name << "' not found in tree" << std::endl;
            }
          }

          m_data = boost::any(data_type{});
          data_type* data = &boost::any_cast<data_type&>(m_data);
          data->reserve(std::size_t(maxsize));

          m_data_resize = [data] ( std::size_t len ) {
            if ( len > data->capacity() ) {
              len = data->capacity();
            }
            data->resize(len);
          };

          if ( m_tree.tree() ) {
            if ( m_branch ) {
              TLeaf* cntLeaf = m_branch->GetLeaf(m_name.c_str())->GetLeafCount();
              if ( ! cntLeaf ) {
                std::cout << "ERROR: No count leaf for '" << m_name << "', try accessing as std::vector directly" << std::endl;
              }
              if ( cntLeaf->GetName() != m_lengthLeafName ) {
                std::cout << "ERROR: count leaf in tree is " << cntLeaf->GetName() << " while this leaf was created from " << m_lengthLeafName << std::endl;
              }

              m_tree.tree()->SetBranchAddress<T>(m_name.c_str(), data->data(), &m_branch);
            } else {
              m_brancher.reset(new VarrBranchReaderT<T>(data->data(), &m_branch, m_lengthLeafName));
            }
          }

          if ( m_branch ) {
            // Enable read for this branch
            ROOT::utils::activateBranch(m_branch);

            if ( m_tree.entry() != -1 ) {
              // A global GetEntry already happened in the tree
              // Call GetEntry directly on the Branch to catch up
              m_branch->GetEntry(m_tree.entry());
            }
          }
        }

        return const_cast<const data_type&>(boost::any_cast<data_type&>(m_data));
      }

    private:
      void getEntry(uint64_t entry, std::size_t len, bool readall)
      {
        m_data_resize(len);
        if ( ! readall ) {
          m_branch->GetEntry(entry);
        }
      }

      void init(const TreeWrapperAccessor& tree) {
        m_tree = tree;
        if ( m_brancher.get()) {
          (*m_brancher)(m_name, m_tree.tree());
        }
      }

      TBranch* getBranch() const { return m_branch; }

    private:
      friend class TreeWrapper;
      friend class VarrGroup;

      boost::any m_data;
      std::function<void(std::size_t)> m_data_resize;

      void* m_data_ptr = nullptr;

      TBranch* m_branch = nullptr;

      std::string m_name;
      std::string m_lengthLeafName;
      TreeWrapperAccessor m_tree;

      std::unique_ptr<Brancher> m_brancher;
  };

  class VarrGroup {
    friend class TreeWrapper;
    public:
      bool has(const std::string& name) const { return m_leafs.count(name); }

      VarrLeaf& operator[]( const std::string& name)
      {
        if ( has(name) ) {
          return *(m_leafs.at(name));
        }

        std::shared_ptr<VarrLeaf> leaf{new VarrLeaf(name, m_lengthLeaf->name(), &m_wrapper)};
        m_leafs[name] = leaf;

        return *leaf;
      }

      void getEntry(uint64_t entry, bool readall)
      {
        if ( ! readall ) {
          m_lengthLeaf->m_branch->GetEntry(entry);
        }
        const std::size_t len = m_lengthReader->get();
        for ( const auto& ilf : m_leafs ) {
          ilf.second->getEntry(entry, len, readall);
        }
      }

      void init(const TreeWrapperAccessor& tree) {
        m_lengthLeaf->init(tree);
        for ( auto& ilf : m_leafs ) {
          ilf.second->init(tree);
        }
      }
    private:
      class LengthReader {
      public:
        virtual ~LengthReader() {}
        virtual std::size_t get() const = 0;
      private:
      };

      VarrGroup(std::shared_ptr<Leaf> lengthLeaf, TreeWrapper& wrapper, typename std::unique_ptr<LengthReader>&& lengthReader) :
        m_lengthLeaf(lengthLeaf),
        m_wrapper(wrapper),
        m_lengthReader(std::move(lengthReader))
      {}
    private:
      std::shared_ptr<Leaf> m_lengthLeaf;
      std::unordered_map<std::string, std::shared_ptr<VarrLeaf>> m_leafs;
      TreeWrapper& m_wrapper;

      std::unique_ptr<LengthReader> m_lengthReader;
    private:
      template<typename S>
      class LengthReaderT : public LengthReader {
      public:
        LengthReaderT(Leaf& lenLeaf) : m_data{lenLeaf.read<S>()} {}
        virtual ~LengthReaderT() {}
        //
        std::size_t get() const override { return m_data; }
      private:
        const S& m_data;
      };

      template<typename S>
      static std::shared_ptr<VarrGroup> create(std::shared_ptr<Leaf> lengthLeaf, TreeWrapper& wrapper)
      {
        return std::shared_ptr<VarrGroup>(new VarrGroup(lengthLeaf, wrapper, std::unique_ptr<LengthReader>(new LengthReaderT<S>(*lengthLeaf))));
      }
  };
}
