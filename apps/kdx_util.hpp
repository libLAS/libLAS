#ifndef KDX_UTIL_HPP_INCLUDED
#define KDX_UTIL_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include <liblas/lasbounds.hpp>

#include <fstream>
#include <exception>
#include <vector>
#include <sys/stat.h>




typedef std::vector<uint32_t> IDVector;
typedef boost::shared_ptr< IDVector > IDVectorPtr;

class IndexResult 
{
public:
    IndexResult(uint32_t id) : bounds(), m_id(id) {}

    IDVector const& GetIDs() const { return ids; }
    void SetIDs(IDVector& v) {ids = v;}
    const liblas::Bounds GetBounds() const { return bounds; }
    void SetBounds(const liblas::Bounds b) {bounds = b;}
    uint32_t GetID() const {return m_id;}
    void SetID(uint32_t v) {m_id = v;}

private:

    // /// Copy constructor.
    // IndexResult(IndexResult const& other);
    // 
    // /// Assignment operator.
    // IndexResult& operator=(IndexResult const& rhs);
    
    IDVector ids;
    liblas::Bounds bounds;
    uint32_t m_id;

};

typedef std::vector<IndexResult> ResultsVector;
class KDXIndexSummary
{
public:
    KDXIndexSummary(std::istream& input);    
    boost::shared_ptr<liblas::Bounds> bounds;
    ResultsVector& GetResults() { return m_results; }
private:
    ResultsVector m_results;
    bool m_first;    
};

bool KDTreeIndexExists(std::string& filename);


#endif // KDX_UTIL_HPP_INCLUDED

