#include "kdx_util.hpp"



KDXIndexSummary::KDXIndexSummary(std::istream& input) :  bounds(), m_first(true)
{
    long id_count = 0;
    long id = 0;
    long i = 0;

    
    double low[2];
    double high[2];
    
    double mins[2];
    double maxs[2];
    
    bool first = true;
    
    while(input) {
        input >> id >> id_count >> low[0] >> low[1] >> high[0] >> high[1];
        // printf("count:%d %.2f %.2f %.2f %.2f\n", id_count, low[0], low[1], high[0], high[1]);
        
        if (first) {
            mins[0] = low[0];
            mins[1] = low[1];
            maxs[0] = high[0];
            maxs[1] = high[1];
            first = false;
        }
        
        mins[0] = std::min(mins[0], low[0]);
        mins[1] = std::min(mins[1], low[1]);
        
        maxs[0] = std::max(maxs[0], high[0]);
        maxs[1] = std::max(maxs[1], high[1]);
        // if (!input.good()) continue;
        
        IDVector ids;
        for (int j=0; j<id_count; j++) {
            input >> i;
            ids.push_back(i);
        }
        liblas::Bounds b(low[0], low[1], high[0],high[1]);
        // SpatialIndex::Region* pr = new SpatialIndex::Region(low, high, 2);
        // printf("Ids size: %d %.3f\n", ids.size(), pr->getLow(0));
        IndexResult result(static_cast<uint32_t>(id));
        result.SetIDs(ids);
        result.SetBounds(b);
        m_results.push_back(result);
    }

    bounds = boost::shared_ptr<liblas::Bounds>(new liblas::Bounds(mins[0], mins[1], maxs[0], maxs[1]));
}

bool KDTreeIndexExists(std::string& filename)
{
    struct stat stats;
    std::ostringstream os;
    os << filename << ".kdx";

    std::string indexname = os.str();
    
    // ret is -1 for no file existing and 0 for existing
    int ret = stat(indexname.c_str(),&stats);

    bool output = false;
    if (ret == 0) output= true; else output =false;
    return output;
}

                    
