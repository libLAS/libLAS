#include "kdx_util.hpp"

KDXIndexSummary::KDXIndexSummary(liblas::Reader& reader, boost::uint32_t capacity, bool verbose)
{
    boost::ignore_unused_variable_warning(verbose);    
    liblas::chipper::Chipper c(&reader, capacity);


    c.Chip();


   double mins[2];
   double maxs[2];
   
   bool first = true;
   
   boost::uint32_t num_blocks = c.GetBlockCount();
      
   for ( boost::uint32_t i = 0; i < num_blocks; ++i )
   {
        const liblas::chipper::Block& b = c.GetBlock(i);

        std::vector<boost::uint32_t> ids = b.GetIDs();       
        liblas::Bounds<double> const& bnd = b.GetBounds();
       if (first) {
           mins[0] = bnd.min(0);
           mins[1] = bnd.min(1);
           maxs[0] = bnd.max(0);
           maxs[1] = bnd.max(1);
           first = false;
       }
       
       mins[0] = (std::min)(mins[0], bnd.min(0));
       mins[1] = (std::min)(mins[1], bnd.min(1));
       
       maxs[0] = (std::max)(maxs[0], bnd.max(0));
       maxs[1] = (std::max)(maxs[1], bnd.max(1));

       IndexResult result(static_cast<boost::uint32_t>(i));
       result.SetIDs(ids);
       result.SetBounds(bnd);
       m_results.push_back(result);
   }

   bounds = boost::shared_ptr<liblas::Bounds<double > >(new liblas::Bounds<double>(mins[0], mins[1], maxs[0], maxs[1]));

}

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
        
        mins[0] = (std::min)(mins[0], low[0]);
        mins[1] = (std::min)(mins[1], low[1]);
        
        maxs[0] = (std::max)(maxs[0], high[0]);
        maxs[1] = (std::max)(maxs[1], high[1]);
        // if (!input.good()) continue;
        
        IDVector ids;
        for (int j=0; j<id_count; j++) {
            input >> i;
            ids.push_back(i);
        }
        liblas::Bounds<double> b(low[0], low[1], high[0],high[1]);

        IndexResult result(static_cast<boost::uint32_t>(id));
        result.SetIDs(ids);
        result.SetBounds(b);
        m_results.push_back(result);
    }

    bounds = boost::shared_ptr<liblas::Bounds<double > >(new liblas::Bounds<double>(mins[0], mins[1], maxs[0], maxs[1]));
}

bool GetPointData(  liblas::Point const& p, 
                    // bool bTime, 
                    std::vector<boost::uint8_t>& point_data)
{
    // This function returns an array of bytes describing the 
    // x,y,z and optionally time values for the point.  

    point_data.clear();

    // Full 8-byte dimension - Dim #1
    double x = p.GetX();

    // Full 8-byte dimension - Dim #2
    double y = p.GetY();

    // Full 8-byte dimension - Dim #3
    double z = p.GetZ();

    // Full 8-byte dimension - Dim #4
    double t = p.GetTime();

    // Full 8-byte dimension - Dim #5
    double c = static_cast<double>(p.GetClassification().GetClass());

    // Full 8-byte dimension - Dim #6
    double intensity = static_cast<double>(p.GetIntensity());
    // double intensity = 34.0;

    // Composed 8-byte dimension - Dim #7
    boost::int8_t returnNumber = (boost::int8_t)p.GetReturnNumber(); // 1 byte
    boost::int8_t numberOfReturns = (boost::int8_t)p.GetNumberOfReturns(); // 1 byte
    boost::int8_t scanDirFlag = (boost::int8_t)p.GetScanDirection(); // 1 byte
    boost::int8_t edgeOfFlightLine = (boost::int8_t)p.GetFlightLineEdge(); // 1 byte
    boost::int8_t scanAngleRank = p.GetScanAngleRank(); // 1 byte
    boost::uint8_t userData = p.GetUserData(); // 1 byte
    boost::uint16_t pointSourceId = p.GetPointSourceID(); // 2 bytes


    // Composed 8-byte dimension - Dim #8
    liblas::Color const& color = p.GetColor();
    boost::uint16_t red = color.GetRed();
    boost::uint16_t green = color.GetGreen();
    boost::uint16_t blue = color.GetBlue();
    boost::uint16_t alpha = 0;
    

    boost::uint8_t* x_b =  reinterpret_cast<boost::uint8_t*>(&x);
    boost::uint8_t* y_b =  reinterpret_cast<boost::uint8_t*>(&y);
    boost::uint8_t* z_b =  reinterpret_cast<boost::uint8_t*>(&z);
    boost::uint8_t* t_b =  reinterpret_cast<boost::uint8_t*>(&t);
    boost::uint8_t* c_b =  reinterpret_cast<boost::uint8_t*>(&c);
    
    boost::uint8_t* intensity_b =  reinterpret_cast<boost::uint8_t*>(&intensity);
    
    boost::uint8_t* returnNumber_b =  reinterpret_cast<boost::uint8_t*>(&returnNumber);
    boost::uint8_t* numberOfReturns_b =  reinterpret_cast<boost::uint8_t*>(&numberOfReturns);
    boost::uint8_t* scanDirFlag_b =  reinterpret_cast<boost::uint8_t*>(&scanDirFlag);
    boost::uint8_t* edgeOfFlightLine_b =  reinterpret_cast<boost::uint8_t*>(&edgeOfFlightLine);
    boost::uint8_t* scanAngleRank_b =  reinterpret_cast<boost::uint8_t*>(&scanAngleRank);
    boost::uint8_t* userData_b =  reinterpret_cast<boost::uint8_t*>(&userData);
    boost::uint8_t* pointSourceId_b =  reinterpret_cast<boost::uint8_t*>(&pointSourceId);


    boost::uint8_t* red_b =  reinterpret_cast<boost::uint8_t*>(&red);
    boost::uint8_t* green_b =  reinterpret_cast<boost::uint8_t*>(&green);
    boost::uint8_t* blue_b =  reinterpret_cast<boost::uint8_t*>(&blue);
    boost::uint8_t* alpha_b =  reinterpret_cast<boost::uint8_t*>(&alpha);
 

    // big-endian
    for (int i = sizeof(double) - 1; i >= 0; i--) {
        point_data.push_back(x_b[i]);
    }

    for (int i = sizeof(double) - 1; i >= 0; i--) {
        point_data.push_back(y_b[i]);
    }   

    for (int i = sizeof(double) - 1; i >= 0; i--) {
        point_data.push_back(z_b[i]);
    }
    
    for (int i = sizeof(double) - 1; i >= 0; i--) {
        point_data.push_back(t_b[i]);
    }

    // Classification is only a single byte, but 
    // we need to store it in an 8 byte big-endian 
    // double to satisfy OPC
    for (int i = sizeof(double) - 1; i >= 0; i--) {
        point_data.push_back(c_b[i]);
    }

    // Intensity is only two bytes, but 
    // we need to store it in an 8 byte big-endian 
    // double to satisfy OPC
    for (int i = sizeof(double) - 1; i >= 0; i--) {
        point_data.push_back(intensity_b[i]);
    }


    // Pack dimension with a number of fields totalling 8 bytes
    point_data.push_back(returnNumber_b[0]);
    point_data.push_back(numberOfReturns_b[0]);
    point_data.push_back(scanDirFlag_b[0]);
    point_data.push_back(edgeOfFlightLine_b[0]);
    point_data.push_back(scanAngleRank_b[0]);
    point_data.push_back(userData_b[0]);
    for (int i = sizeof(boost::uint16_t) - 1; i >= 0; i--) {
        point_data.push_back(pointSourceId_b[i]);
    }

    // Pack dimension with RGBA for a total of 8 bytes
    for (int i = sizeof(boost::uint16_t) - 1; i >= 0; i--) {
        point_data.push_back(red_b[i]);
    }    
    for (int i = sizeof(boost::uint16_t) - 1; i >= 0; i--) {
        point_data.push_back(green_b[i]);
    }    
    for (int i = sizeof(boost::uint16_t) - 1; i >= 0; i--) {
        point_data.push_back(blue_b[i]);
    }    
    for (int i = sizeof(boost::uint16_t) - 1; i >= 0; i--) {
        point_data.push_back(alpha_b[i]);
    }    

    
    return true;
}

void IndexResult::GetData(liblas::Reader* reader, std::vector<boost::uint8_t>& data)
{
    IDVector const& ids = GetIDs();


    // d 8-byte IEEE  big-endian doubles, where d is the PC_TOT_DIMENSIONS value
    // 4-byte big-endian integer for the BLK_ID value
    // 4-byte big-endian integer for the PT_ID value
    
    
    data.clear(); 
    
    IDVector::const_iterator i;
    std::vector<boost::uint8_t>::iterator pi;
    
    boost::uint32_t block_id = GetID();

    std::vector<boost::uint8_t> point_data;
    
    for (i=ids.begin(); i!=ids.end(); ++i) 
    {
        boost::uint32_t id = *i;

        bool doRead = reader->ReadPointAt(id);
        if (doRead) {
            liblas::Point const& p = reader->GetPoint();

            // d 8-byte IEEE  big-endian doubles, where d is the PC_TOT_DIMENSIONS value
            
            
            bool gotdata = GetPointData(p, point_data);
            
            if (!gotdata) { throw std::runtime_error("Unable to fetch Point Data"); }
            std::vector<boost::uint8_t>::const_iterator d;
            for (d = point_data.begin(); d!=point_data.end(); ++d) {
                data.push_back(*d);
            }

            boost::uint8_t* id_b = reinterpret_cast<boost::uint8_t*>(&id);
            boost::uint8_t* block_b = reinterpret_cast<boost::uint8_t*>(&block_id);
            
            // 4-byte big-endian integer for the BLK_ID value
            for (int i =  sizeof(boost::uint32_t) - 1; i >= 0; i--) {
                data.push_back(block_b[i]);
            }
            
            // 4-byte big-endian integer for the PT_ID value
            for (int i =  sizeof(boost::uint32_t) - 1; i >= 0; i--) {
                data.push_back(id_b[i]);
            }
            

        }
    }

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

                    
