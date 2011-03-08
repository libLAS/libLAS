#ifndef LIBLAS_CHIPPER_H
#define LIBLAS_CHIPPER_H

#include <liblas/liblas.hpp>
#include <liblas/export.hpp>

#include <vector>

namespace liblas
{

namespace chipper
{

enum Direction
{
    DIR_X,
    DIR_Y,
    DIR_NONE
};

class LAS_DLL PtRef
{
public:
    double m_pos;
    boost::uint32_t m_ptindex;
    boost::uint32_t m_oindex;

    bool operator < (const PtRef& pt) const
        { return m_pos < pt.m_pos; }
};

struct LAS_DLL RefList
{
public:
    std::vector<PtRef> m_vec;
    Direction m_dir;

    RefList(Direction dir = DIR_NONE) : m_dir(dir)
        {}
    std::vector<PtRef>::size_type size() const
        { return m_vec.size(); }
    void reserve(std::vector<PtRef>::size_type n)
        { m_vec.reserve(n); }
    void resize(std::vector<PtRef>::size_type n)
        { m_vec.resize(n); }
    void push_back(const PtRef& ref)
        { m_vec.push_back(ref); }
    std::vector<PtRef>::iterator begin()
        { return m_vec.begin(); }
    std::vector<PtRef>::iterator end()
        { return m_vec.end(); }
    PtRef& operator[](boost::uint32_t pos)
        { return m_vec[pos]; }
    std::string Dir()
    {
        if (m_dir == DIR_X)
            return "X";
        else if (m_dir == DIR_Y)
            return "Y";
        else
            return "NONE";
    }
    void SortByOIndex(boost::uint32_t left, boost::uint32_t center,
        boost::uint32_t right);
};

class LAS_DLL Chipper;

class LAS_DLL Block
{
    friend class Chipper;

private:
    RefList *m_list_p;
    boost::uint32_t m_left;
    boost::uint32_t m_right;
    liblas::Bounds<double> m_bounds;

public:
    std::vector<boost::uint32_t> GetIDs() const; 
    Bounds<double> const& GetBounds() const
        {return m_bounds;} 
    void SetBounds(liblas::Bounds<double> const& bounds)
        {m_bounds = bounds;}
};

// Options that can be used to modify the behavior of the chipper.
class LAS_DLL Options
{
public:
    Options() : m_threshold( 1000 ), m_use_sort( false ),
       m_use_maps( false )
    {}

    // Maximum number of pointer per output block.
    boost::uint32_t m_threshold;
    // If true, use sorting instead of copying to reduce memory.
    bool m_use_sort;
    // If true, use memory mapped files instead of main memory
    // (not currently impelemented).
    bool m_use_maps;
    // Directory to hold map files.
    std::string m_map_dir;
};

class LAS_DLL Chipper
{
public:
    Chipper(Reader *reader, Options *options );
    Chipper(Reader *reader, boost::uint32_t max_partition_size) :
        m_reader(reader), m_xvec(DIR_X), m_yvec(DIR_Y), m_spare(DIR_NONE)
    {
        m_options.m_threshold = max_partition_size;
    }

    void Chip();
    std::vector<Block>::size_type GetBlockCount()
        { return m_blocks.size(); }
    const Block& GetBlock(std::vector<Block>::size_type i)
        { return m_blocks[i]; }

private:
    void Allocate();
    void Load();
    void Partition(boost::uint32_t size);
    void Split(RefList& xvec, RefList& yvec, RefList& spare);
    void DecideSplit(RefList& v1, RefList& v2, RefList& spare,
        boost::uint32_t left, boost::uint32_t right);
    void RearrangeNarrow(RefList& wide, RefList& narrow, RefList& spare,
        boost::uint32_t left, boost::uint32_t center, boost::uint32_t right);
    void Split(RefList& wide, RefList& narrow, RefList& spare,
        boost::uint32_t left, boost::uint32_t right);
    void FinalSplit(RefList& wide, RefList& narrow,
        boost::uint32_t pleft, boost::uint32_t pcenter);
    void Emit(RefList& wide, boost::uint32_t widemin, boost::uint32_t widemax,
        RefList& narrow, boost::uint32_t narrowmin, boost::uint32_t narrowmax);

    Reader *m_reader;
    std::vector<Block> m_blocks;
    std::vector<boost::uint32_t> m_partitions;
    RefList m_xvec;
    RefList m_yvec;
    RefList m_spare;
    Options m_options;
};

} // namespace chipper

} // namespace liblas

#endif
