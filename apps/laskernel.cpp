
#include "laskernel.hpp"

bool IsDualRangeFilter(std::string parse_string) 
{

string::size_type dash = parse_string.find_first_of("-");

if (dash != std::string::npos) {
    return true;
}
return false;
}

liblas::FilterI*  MakeReturnFilter(std::string return_string, liblas::FilterI::FilterType ftype) 
{
    boost::char_separator<char> sep(SEPARATORS);

    std::vector<boost::uint16_t> returns;
    tokenizer tokens(return_string, sep);
    for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
        returns.push_back(atoi((*t).c_str()));
    }

    liblas::ReturnFilter* return_filter = new liblas::ReturnFilter(returns, false);
    return_filter->SetType(ftype);
    return return_filter;
}


liblas::FilterI*  MakeClassFilter(std::string class_string, liblas::FilterI::FilterType ftype) 
{
    boost::char_separator<char> sep(SEPARATORS);

    std::vector<boost::uint8_t> classes;
    tokenizer tokens(class_string, sep);
    for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
        classes.push_back(atoi((*t).c_str()));
    }

    liblas::ClassificationFilter* class_filter = new liblas::ClassificationFilter(classes); 
    class_filter->SetType(ftype);
    return class_filter;
}

liblas::FilterI*  MakeBoundsFilter(std::string bounds_string, liblas::FilterI::FilterType ftype) 
{
    boost::char_separator<char> sep(SEPARATORS);
    std::vector<double> vbounds;
    tokenizer tokens(bounds_string, sep);
    liblas::Bounds<double> bounds;
    for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
        vbounds.push_back(atof((*t).c_str()));
    }
    if (vbounds.size() == 4) 
    {
        bounds = liblas::Bounds<double>(vbounds[0], 
                                vbounds[1], 
                                vbounds[2], 
                                vbounds[3]);
    } else if (vbounds.size() == 6)
    {
        bounds = liblas::Bounds<double>(vbounds[0], 
                                vbounds[1], 
                                vbounds[2], 
                                vbounds[3], 
                                vbounds[4], 
                                vbounds[5]);
    } else {
        ostringstream oss;
        oss << "Bounds must be specified as a 4-tuple or "
               "6-tuple, not a "<< vbounds.size()<<"-tuple" << "\n";
        throw std::runtime_error(oss.str());
    }
    liblas::BoundsFilter* bounds_filter = new liblas::BoundsFilter(bounds);
    bounds_filter->SetType(ftype);
    return bounds_filter;
}

liblas::FilterI*  MakeIntensityFilter(std::string intensities, liblas::FilterI::FilterType ftype) 
{
    liblas::ContinuousValueFilter<boost::uint16_t>::filter_func f = &liblas::Point::GetIntensity;
    liblas::ContinuousValueFilter<boost::uint16_t>* intensity_filter = new liblas::ContinuousValueFilter<boost::uint16_t>(f, intensities);
    intensity_filter->SetType(ftype);
    return intensity_filter;
}

liblas::FilterI*  MakeTimeFilter(std::string times, liblas::FilterI::FilterType ftype) 
{
    liblas::ContinuousValueFilter<double>::filter_func f = &liblas::Point::GetTime;
    liblas::ContinuousValueFilter<double>* time_filter = new liblas::ContinuousValueFilter<double>(f, times);
    time_filter->SetType(ftype);
    return time_filter;
}

liblas::FilterI*  MakeScanAngleFilter(std::string intensities, liblas::FilterI::FilterType ftype) 
{
    liblas::ContinuousValueFilter<boost::int8_t>::filter_func f = &liblas::Point::GetScanAngleRank;
    liblas::ContinuousValueFilter<boost::int8_t>* intensity_filter = new liblas::ContinuousValueFilter<boost::int8_t>(f, intensities);
    intensity_filter->SetType(ftype);
    return intensity_filter;
}

liblas::FilterI* MakeColorFilter(liblas::Color const& low, liblas::Color const& high, liblas::FilterI::FilterType ftype)
{
    liblas::ColorFilter* filter = new liblas::ColorFilter(low, high);
    filter->SetType(ftype);
    return filter;
}

po::options_description GetFilteringOptions() 
{

po::options_description filtering_options("Filtering options");

filtering_options.add_options()
    ("extent,e", po::value< string >(), "Extent window that points must fall within to keep.\nUse a comma-separated list, for example, \n  -e minx, miny, maxx, maxy\n  or \n  -e minx, miny, minz, maxx, maxy, maxz")
    ("thin,t", po::value<boost::uint32_t>()->default_value(0), "Simple decimation-style thinning.\nThin the file by removing every t'th point from the file.")
    ("last_return_only", po::value<bool>()->zero_tokens(), "Keep last returns (cannot be used with --first_return_only)")
    ("first_return_only", po::value<bool>()->zero_tokens(), "Keep first returns (cannot be used with --last_return_only")
    ("keep-returns", po::value< string >(), "A comma-separated list of return numbers to keep in the output file: \n--keep-returns 1,2,3")
    ("drop-returns", po::value< string >(), "Return numbers to drop.\nUse a comma-separated list, for example, --drop-returns 2,3,4,5")
    ("valid_only", po::value<bool>()->zero_tokens(), "Keep only valid points")
    ("keep-classes", po::value< string >(), "A comma-separated list of classifications to keep:\n--keep-classes 2,4,12\n--keep-classes 2")
    ("drop-classes", po::value< string >(), "A comma-separated list of classifications to drop:\n--drop-classes 1,7,8\n--drop-classes 2")
    ("keep-intensity", po::value< string >(), "Range in which to keep intensity.\nThe following expression types are supported:  \n--keep-intensity 0-100 \n--keep-intensity <200 \n--keep-intensity >400 \n--keep-intensity >=200")
    ("drop-intensity", po::value< string >(), "Range in which to drop intensity.\nThe following expression types are supported:  \n--drop-intensity <200 \n--drop-intensity >400 \n--drop-intensity >=200")
    ("keep-time", po::value< string >(), "Range in which to keep time.\nThe following expression types are supported:  \n--keep-time 413665.2336-414092.8462 \n--keep-time <414094.8462 \n--keep-time >413665.2336 \n--keep-time >=413665.2336")
    ("drop-time", po::value< string >(), "Range in which to drop time.\nThe following expression types are supported:  \n--drop-time <413666.2336 \n--drop-time >413665.2336 \n--drop-time >=413665.2336")
    ("keep-scan-angle", po::value< string >(), "Range in which to keep scan angle.\nThe following expression types are supported:  \n--keep-scan-angle 0-100 \n--keep-scan-angle <100\n--keep-scan-angle <=100")
    ("drop-scan-angle", po::value< string >(), "Range in which to drop scan angle.\nThe following expression types are supported:  \n--drop-scan-angle <30 \n--drop-scan-angle >100 \n--drop-scan-angle >=100")
    ("keep-color", po::value< string >(), "Range in which to keep colors.\nDefine colors as two 3-tuples (R,G,B-R,G,B):  \n--keep-color '0,0,0-125,125,125'")
    ("drop-color", po::value< string >(), "Range in which to drop colors.\nDefine colors as two 3-tuples (R,G,B-R,G,B):  \n--drop-color '255,255,255-65536,65536,65536'")
;
return filtering_options;    
}

po::options_description GetTransformationOptions() 
{
    po::options_description transform_options("Transformation options");

    transform_options.add_options()
        ("a_srs", po::value< string >(), "Coordinate system to assign to input LAS file")
        ("t_srs", po::value< string >(), "Coordinate system to reproject output LAS file to.  Use --a_srs or verify that your input LAS file has a coordinate system according to lasinfo")   
        ("offset", po::value< string >(), "A comma-separated list of offsets to set on the output file: \n--offset 0,0,0 \n--offset  min,min,min")
        ("scale", po::value< string >(), "A comma-separated list of scales to set on the output file: \n--scale 0.1,0.1,0.00001")
        ("format,f", po::value< string >(), "Set the LAS format of the new file (only 1.0-1.2 supported at this time): \n--format 1.2\n-f 1.1")
        ("pad-header", po::value< string >(), "Add extra bytes to the existing header")

    ;
    
    return transform_options;
}

std::vector<liblas::FilterI*> GetFilters(po::variables_map vm, bool verbose)
{
    std::vector<liblas::FilterI*> filters;
    
    if (vm.count("keep-classes")) 
    {
        std::string classes = vm["keep-classes"].as< string >();
        if (verbose)
            std::cout << "Keeping classes with the values: " << classes << std::endl;
            
        liblas::FilterI* class_filter = MakeClassFilter(  classes, 
                                                          liblas::FilterI::eInclusion);
        filters.push_back(class_filter); 
    }

    if (vm.count("drop-classes")) 
    {
        std::string classes = vm["drop-classes"].as< string >();
        if (verbose)
            std::cout << "Dropping classes with the values: " << classes << std::endl;
            
        liblas::FilterI* class_filter = MakeClassFilter(  classes, 
                                                            liblas::FilterI::eExclusion);
        filters.push_back(class_filter);
    }

    if (vm.count("keep-returns")) 
    {
        std::string returns = vm["keep-returns"].as< string >();
        if (verbose)
            std::cout << "Keeping returns with the values: " << returns << std::endl;
            
        liblas::FilterI* return_filter = MakeReturnFilter(  returns, 
                                                            liblas::FilterI::eInclusion);
        filters.push_back(return_filter); 
    }

    if (vm.count("drop-returns")) 
    {
        std::string returns = vm["drop-returns"].as< string >();
        if (verbose)
            std::cout << "Dropping returns with the values: " << returns << std::endl;
            
        liblas::FilterI* return_filter = MakeReturnFilter(  returns, 
                                                            liblas::FilterI::eExclusion);
        filters.push_back(return_filter); 
    }
            
    if (vm.count("extent")) 
    {
        std::string bounds = vm["extent"].as< string >();
        if (verbose)
            std::cout << "Clipping file to the extent : " << bounds << std::endl;
        liblas::FilterI* bounds_filter = MakeBoundsFilter(bounds, liblas::FilterI::eInclusion);
        filters.push_back(bounds_filter);
        
    }
    if (vm.count("keep-intensity")) 
    {
        std::string intensities = vm["keep-intensity"].as< string >();
        if (verbose)
            std::cout << "Keeping intensities with values: " << intensities << std::endl;
        if (IsDualRangeFilter(intensities)) {
            // We need to make two filters
            // Given a range 0-200, split the expression into two filters 
            string::size_type dash = intensities.find_first_of("-");
            std::string low = intensities.substr(0,dash);
            std::string high = intensities.substr(dash+1, intensities.size());

            liblas::FilterI* lt_filter = MakeIntensityFilter(">="+low, liblas::FilterI::eInclusion);
            filters.push_back(lt_filter);
            liblas::FilterI* gt_filter = MakeIntensityFilter("<="+high, liblas::FilterI::eInclusion);
            filters.push_back(gt_filter);                
        } else {
            liblas::FilterI* intensity_filter = MakeIntensityFilter(intensities, liblas::FilterI::eInclusion);
            filters.push_back(intensity_filter);
            
        }
    }
    if (vm.count("drop-intensity")) 
    {
        std::string intensities = vm["drop-intensity"].as< string >();
        if (verbose)
            std::cout << "Dropping intensities with values: " << intensities << std::endl;

        if (IsDualRangeFilter(intensities)) {
            throw std::runtime_error("Range filters are not supported for drop-intensity");
        } else {
            liblas::FilterI* intensity_filter = MakeIntensityFilter(intensities, liblas::FilterI::eExclusion);
            filters.push_back(intensity_filter);   
        }
    }
    if (vm.count("keep-scan-angle")) 
    {
        std::string angles = vm["keep-scan-angle"].as< string >();
        if (verbose)
            std::cout << "Keeping scan angles with values: " << angles << std::endl;
        if (IsDualRangeFilter(angles)) {
            // We need to make two filters
            // Given a range 0-200, split the expression into two filters 
            string::size_type dash = angles.find_first_of("-");
            std::string low = angles.substr(0,dash);
            std::string high = angles.substr(dash+1, angles.size());

            liblas::FilterI* lt_filter = MakeScanAngleFilter(">="+low, liblas::FilterI::eInclusion);
            filters.push_back(lt_filter);
            liblas::FilterI* gt_filter = MakeScanAngleFilter("<="+high, liblas::FilterI::eInclusion);
            filters.push_back(gt_filter);                
        } else {
            liblas::FilterI* angle_filter = MakeScanAngleFilter(angles, liblas::FilterI::eInclusion);
            filters.push_back(angle_filter);
            
        }
    }
    if (vm.count("drop-scan-angle")) 
    {
        std::string angles = vm["drop-scan-angle"].as< string >();
        if (verbose)
            std::cout << "Dropping scan angles with values: " << angles << std::endl;

        if (IsDualRangeFilter(angles)) {
            throw std::runtime_error("Range filters are not supported for drop-scan-angle");
        } else {
            liblas::FilterI* angle_filter = MakeScanAngleFilter(angles, liblas::FilterI::eExclusion);
            filters.push_back(angle_filter);   
        }
    }
    
    if (vm.count("keep-time")) 
    {
        std::string times = vm["keep-time"].as< string >();
        if (verbose)
            std::cout << "Keeping times with values: " << times << std::endl;
        if (IsDualRangeFilter(times)) {
            // We need to make two filters
            // Given a range 0-200, split the expression into two filters 
            string::size_type dash = times.find_first_of("-");
            std::string low = times.substr(0,dash);
            std::string high = times.substr(dash+1, times.size());

            liblas::FilterI* lt_filter = MakeTimeFilter(">="+low, liblas::FilterI::eInclusion);
            filters.push_back(lt_filter);
            liblas::FilterI* gt_filter = MakeTimeFilter("<="+high, liblas::FilterI::eInclusion);
            filters.push_back(gt_filter);                
        } else {
            liblas::FilterI* time_filter = MakeTimeFilter(times, liblas::FilterI::eInclusion);
            filters.push_back(time_filter);
            
        }
    }
    if (vm.count("drop-time")) 
    {
        std::string times = vm["drop-time"].as< string >();
        if (verbose)
            std::cout << "Dropping times with values: " << times << std::endl;
            
        if (IsDualRangeFilter(times)) {
            throw std::runtime_error("Range filters are not supported for drop-time");
        } else {
            liblas::FilterI* time_filter = MakeTimeFilter(times, liblas::FilterI::eExclusion);
            filters.push_back(time_filter);   
        }
    }

    if (vm.count("keep-color")) 
    {
        std::string keepers = vm["keep-color"].as< string >();
        if (verbose)
            std::cout << "Keeping colors in range:: " << keepers << std::endl;
            
        // Pull apart color ranges in the form: R,G,B-R,G,B
        boost::char_separator<char> sep_dash("-");
        boost::char_separator<char> sep_comma(",");
        std::vector<liblas::Color> colors;
        tokenizer low_high(keepers, sep_dash);
        for (tokenizer::iterator t = low_high.begin(); t != low_high.end(); ++t) {
            
            tokenizer rgbs((*t), sep_comma);
            std::vector<liblas::Color::value_type> rgb;
            for(tokenizer::iterator c = rgbs.begin(); c != rgbs.end(); ++c)
            {
                rgb.push_back(atoi((*c).c_str()));
            }
            liblas::Color color(rgb[0], rgb[1], rgb[2]);
            colors.push_back(color);
        }
        
        liblas::FilterI* color_filter = MakeColorFilter(colors[0], colors[1], liblas::FilterI::eInclusion);
        filters.push_back(color_filter);
    }
    if (vm.count("drop-color")) 
    {
        std::string dropers = vm["drop-color"].as< string >();
        if (verbose)
            std::cout << "Dropping colors in range:: " << dropers << std::endl;
            
        // Pull apart color ranges in the form: R,G,B-R,G,B
        boost::char_separator<char> sep_dash("-");
        boost::char_separator<char> sep_comma(",");
        std::vector<liblas::Color> colors;
        tokenizer low_high(dropers, sep_dash);
        for (tokenizer::iterator t = low_high.begin(); t != low_high.end(); ++t) {
            
            tokenizer rgbs((*t), sep_comma);
            std::vector<liblas::Color::value_type> rgb;
            for(tokenizer::iterator c = rgbs.begin(); c != rgbs.end(); ++c)
            {
                rgb.push_back(atoi((*c).c_str()));
            }
            liblas::Color color(rgb[0], rgb[1], rgb[2]);
            colors.push_back(color);
        }
        
        liblas::FilterI* color_filter = MakeColorFilter(colors[0], colors[1], liblas::FilterI::eExclusion);
        filters.push_back(color_filter);
    }

    if (vm.count("thin")) 
    {
        boost::uint32_t thin = vm["thin"].as< boost::uint32_t >();
        if (thin != 0) {
            if (verbose)
                std::cout << "Thining file by keeping every "<<thin<<"'th point "  << std::endl;
            
            liblas::ThinFilter* thin_filter = new liblas::ThinFilter(thin);
            filters.push_back(thin_filter);    
        }
    }


    if (vm.count("first_return_only") && vm.count("last_return_only")) {
        throw std::runtime_error( "--first_return_only and --last_return_only cannot "
                     "be used simultaneously.  Use --keep-returns 1 in "
                     "combination with --last_return_only");
    }

    if (vm.count("last_return_only")) {
        if (verbose)
            std::cout << "Keeping last returns only."  << std::endl;
        std::vector<boost::uint16_t> returns;
        liblas::ReturnFilter* last_filter = new liblas::ReturnFilter(returns, true);
        filters.push_back(last_filter);
    }

    if (vm.count("first_return_only")){
        if (verbose)
            std::cout << "Keeping first returns only."  << std::endl;
        std::vector<boost::uint16_t> returns;
        returns.push_back(1);
        liblas::ReturnFilter* return_filter = new liblas::ReturnFilter(returns, false);
        filters.push_back(return_filter);
    }
    
    if (vm.count("valid_only")){
        if (verbose)
            std::cout << "Keeping valid points only."  << std::endl;
        liblas::ValidationFilter* valid_filter = new liblas::ValidationFilter();
        filters.push_back(valid_filter);            
    }

    return filters;
}

std::vector<liblas::TransformI*> GetTransforms(po::variables_map vm, bool verbose, liblas::Header& header)
{
    std::vector<liblas::TransformI*> transforms;

    if (vm.count("a_srs")) 
    {
        liblas::SpatialReference in_ref;
        
        std::string input_srs = vm["a_srs"].as< string >();
        if (verbose)
            std::cout << "Setting input SRS to " << input_srs << std::endl;
        in_ref.SetFromUserInput(input_srs);
        header.SetSRS(in_ref);
    }
    
    if (vm.count("t_srs")) 
    {
        liblas::SpatialReference in_ref;
        liblas::SpatialReference out_ref;
        
        std::string output_srs = vm["t_srs"].as< string >();
        if (verbose)
            std::cout << "Setting output SRS to " << output_srs << std::endl;
        out_ref.SetFromUserInput(output_srs);

        if (vm.count("a_srs")){
            std::string input_srs = vm["a_srs"].as< string >();
            in_ref.SetFromUserInput(input_srs);
        } else {
            // If the user didn't assign an input SRS, we'll try to take 
            // it from our existing header.
            in_ref = header.GetSRS();
            if (in_ref.GetVLRs().size() == 0)
            {
                throw std::runtime_error("No input SRS is available on the file you have specified.  Please use --a_srs to assign one");
            }
            
        }
        // Set the header's SRS to the output SRS now.  We've already 
        // made the transformation, and this SRS will be used to 
        // write the new file(s)
        header.SetSRS(out_ref);
        
        liblas::Bounds<double> b = header.GetExtent();
        b.project(in_ref, out_ref);
        header.SetExtent(b);
        liblas::TransformI* srs_transform = new liblas::ReprojectionTransform(in_ref, out_ref);
        transforms.push_back(srs_transform);
    }




    if (vm.count("offset")) 
    {
        std::string offset_string = vm["offset"].as< string >();
        if (verbose)
            std::cout << "Setting offsets to: " << offset_string << std::endl;
        boost::char_separator<char> sep(SEPARATORS);
        std::vector<double> offsets;
        tokenizer tokens(offset_string, sep);
        bool mins = false;
        std::string m("min");
        for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
            // Check if the user set --offset min,min,min
            // FIXME: make this so the user could do --offset min,min,20.00
            if (!(*t).compare(m))
            {   
                mins = true;
                continue;
            }
            else
            {
                mins = false;
                offsets.push_back(atof((*t).c_str()));
            }
        }
        if (offsets.size() != 3) 
        {
            throw std::runtime_error("All three values for setting the offset must be floats");

        }
        header.SetOffset(offsets[0], offsets[1], offsets[2]);
    }

    if (vm.count("scale")) 
    {
        std::string scale_string = vm["scale"].as< string >();
        if (verbose)
            std::cout << "Setting scales to: " << scale_string << std::endl;

        boost::char_separator<char> sep(SEPARATORS);
        std::vector<double> scales;
        tokenizer tokens(scale_string, sep);
        std::string m("min");
        for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
            scales.push_back(atof((*t).c_str()));
        }

        header.SetScale(scales[0], scales[1], scales[2]);
    }
    if (vm.count("format")) 
    {
        std::string format_string = vm["format"].as< string >();
        if (verbose)
            std::cout << "Setting format to: " << format_string << std::endl;
            
        boost::char_separator<char> sep(".");
        std::vector<int> versions;
        tokenizer tokens(format_string, sep);
        for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
            const char* v =(*t).c_str();
            int i = atoi(v);
            versions.push_back(i);
        }
        if (versions.size() < 2)
        {
            ostringstream oss;
            oss << "Format version must dotted -- ie, '1.0' or '1.2', not " << format_string;
            throw std::runtime_error(oss.str());
        }
        
        int minor = versions[1];
        if (minor > 2){
            ostringstream oss;
            oss << "Format version must dotted -- ie, '1.0' or '1.2', not " << format_string;
            throw std::runtime_error(oss.str());
        }
        header.SetVersionMinor(static_cast<boost::uint8_t>(minor)); 
    }
    if (vm.count("pad-header")) 
    {
        std::string header_pad = vm["pad-header"].as< string >();
        if (verbose)
            std::cout << "Increasing header pad to: " << header_pad << std::endl;

        boost::uint32_t offset = header.GetDataOffset();
        if (atoi(header_pad.c_str()) == 0) {
            ostringstream oss;
            oss << "Header pad was 0.  It must be greater than "<<offset<< " bytes";
            throw std::runtime_error(oss.str());
            
        }
        header.SetDataOffset(atoi(header_pad.c_str()));
    }
    
    return transforms;
}


boost::property_tree::ptree SummarizePoints(liblas::Reader& reader )
{
    using boost::property_tree::ptree;
    ptree pt;
    
    boost::array<boost::uint32_t, 32> classes;
    boost::uint32_t synthetic = 0;
    boost::uint32_t withheld = 0;
    boost::uint32_t keypoint = 0;
    boost::uint32_t count = 0;
    boost::array<boost::uint32_t, 8> points_by_return; 
    boost::array<boost::uint32_t, 8> returns_of_given_pulse; 
    
    classes.assign(0);
    points_by_return.assign(0);
    returns_of_given_pulse.assign(0);
        
    bool read = reader.ReadNextPoint();
    if (!read)
    {
        throw std::runtime_error("Unable to read any points from file.");
    }
    
    bool first = true;
    liblas::Point min;
    liblas::Point max;
    
    while (read) 
    {

        count++;
        liblas::Point const& p = reader.GetPoint();

        if (first) {
            min = p;
            max = p;
            first = false;
        }
        
        min.SetX(std::min(p.GetX(), min.GetX()));
        max.SetX(std::max(p.GetX(), max.GetX()));

        min.SetY(std::min(p.GetY(), min.GetY()));
        max.SetY(std::max(p.GetY(), max.GetY()));        

        min.SetZ(std::min(p.GetZ(), min.GetZ()));
        max.SetZ(std::max(p.GetZ(), max.GetZ()));

        min.SetIntensity(std::min(p.GetIntensity(), min.GetIntensity()));
        max.SetIntensity(std::max(p.GetIntensity(), max.GetIntensity()));

        min.SetTime(std::min(p.GetTime(), min.GetTime()));
        max.SetTime(std::max(p.GetTime(), max.GetTime()));

        min.SetReturnNumber(std::min(p.GetReturnNumber(), min.GetReturnNumber()));
        max.SetReturnNumber(std::max(p.GetReturnNumber(), max.GetReturnNumber()));

        min.SetNumberOfReturns(std::min(p.GetNumberOfReturns(), min.GetNumberOfReturns()));
        max.SetNumberOfReturns(std::max(p.GetNumberOfReturns(), max.GetNumberOfReturns()));

        min.SetScanDirection(std::min(p.GetScanDirection(), min.GetScanDirection()));
        max.SetScanDirection(std::max(p.GetScanDirection(), max.GetScanDirection()));

        min.SetFlightLineEdge(std::min(p.GetFlightLineEdge(), min.GetFlightLineEdge()));
        max.SetFlightLineEdge(std::max(p.GetFlightLineEdge(), max.GetFlightLineEdge()));

        min.SetScanAngleRank(std::min(p.GetScanAngleRank(), min.GetScanAngleRank()));
        max.SetScanAngleRank(std::max(p.GetScanAngleRank(), max.GetScanAngleRank()));

        min.SetUserData(std::min(p.GetUserData(), min.GetUserData()));
        max.SetUserData(std::max(p.GetUserData(), max.GetUserData()));

        min.SetPointSourceID(std::min(p.GetPointSourceID(), min.GetPointSourceID()));
        max.SetPointSourceID(std::max(p.GetPointSourceID(), max.GetPointSourceID()));
        
        liblas::Classification const& cls = p.GetClassification();
        
        boost::uint8_t minc = std::min(cls.GetClass(), min.GetClassification().GetClass());
        boost::uint8_t maxc = std::max(cls.GetClass(), max.GetClassification().GetClass());
        
        classes[cls.GetClass()]++;
        
        if (cls.IsWithheld()) withheld++;
        if (cls.IsKeyPoint()) keypoint++;
        if (cls.IsSynthetic()) synthetic++;
        
        min.SetClassification(liblas::Classification(minc));
        max.SetClassification(liblas::Classification(maxc));
        
        liblas::Color const& color = p.GetColor();
        
        liblas::Color::value_type red;
        liblas::Color::value_type green;
        liblas::Color::value_type blue;
        
        red = std::min(color.GetRed(), min.GetColor().GetRed());
        green = std::min(color.GetGreen(), min.GetColor().GetGreen());
        blue = std::min(color.GetBlue(), min.GetColor().GetBlue());
        
        min.SetColor(liblas::Color(red, green, blue));
        
        red = std::max(color.GetRed(), max.GetColor().GetRed());
        green = std::max(color.GetGreen(), max.GetColor().GetGreen());
        blue = std::max(color.GetBlue(), max.GetColor().GetBlue());        

        max.SetColor(liblas::Color(red, green, blue));

        points_by_return[p.GetReturnNumber()]++;
        returns_of_given_pulse[p.GetNumberOfReturns()]++;
        
        read = reader.ReadNextPoint();
    }

    pt.put("bounds.minx", min.GetX());
    pt.put("bounds.miny", min.GetY());
    pt.put("bounds.minz", min.GetZ());
    pt.put("bounds.maxx", max.GetX());
    pt.put("bounds.maxy", max.GetY());
    pt.put("bounds.maxz", max.GetZ());
    
    pt.put("time.min", min.GetTime());
    pt.put("time.max", max.GetTime());
    
    pt.put("intensity.min", min.GetIntensity());
    pt.put("intensity.max", max.GetIntensity());
    
    pt.put("returnnumber.min", min.GetReturnNumber());
    pt.put("returnnumber.max", max.GetReturnNumber());
    
    pt.put("numberofreturns.min", min.GetNumberOfReturns());
    pt.put("numberofreturns.max", max.GetNumberOfReturns());
    
    pt.put("scandirection.min", min.GetScanDirection());
    pt.put("scandirection.max", max.GetScanDirection());
    
    pt.put("scanangle.min", min.GetScanAngleRank());
    pt.put("scanangle.max", max.GetScanAngleRank());
    
    pt.put("flightlineedge.min", min.GetFlightLineEdge());
    pt.put("flightlineedge.max", max.GetFlightLineEdge());
    
    pt.put("userdata.min", min.GetUserData());
    pt.put("userdata.max", max.GetUserData());
    
    pt.put("pointsourceid.min", min.GetPointSourceID());
    pt.put("pointsourceid.max", max.GetPointSourceID());
    
    ptree colors;
    liblas::Color const& c = min.GetColor();
    colors.put("min.red", c.GetRed());
    colors.put("min.green", c.GetGreen());
    colors.put("min.blue", c.GetBlue());
    liblas::Color const& d = max.GetColor();
    colors.put("max.red", d.GetRed());
    if (d.GetGreen() == 57) std::cout << "hey it's 57!" << std::endl;
    colors.put("max.green", d.GetGreen());
    colors.put("max.blue", d.GetBlue());
    pt.add_child("color", colors);
    
    ptree klasses;
    
    for (boost::array<boost::uint32_t,32>::size_type i=0; i < classes.size(); i++) {
        if (classes[i] != 0) {
            liblas::Classification c = liblas::Classification(i, false, false, false);
            std::string name = c.GetClassName();

            klasses.put("name", name);
            klasses.put("count", classes[i]);
            klasses.put("id", i);
            pt.add_child("classification.classification",klasses);            
        }
    }
    pt.put("classification.withheld", withheld);
    pt.put("classification.keypoint", keypoint);
    pt.put("classification.synthetic", synthetic);
    
    ptree returns;
    for (boost::array<boost::uint32_t,8>::size_type i=0; i < points_by_return.size(); i++) {
        if (i == 0) continue;

        if (points_by_return[i] != 0)
        {
            returns.put("id", i);
            returns.put("count", points_by_return[i]);
            pt.add_child("points_by_return.return", returns);
            
        }
    }
    
    ptree pulses;
    for (boost::array<boost::uint32_t,8>::size_type i=0; i < returns_of_given_pulse.size(); i++) {
        if (returns_of_given_pulse[i] != 0) {
            pulses.put("id",i);
            pulses.put("count", returns_of_given_pulse[i]);
            pt.add_child("returns_of_given_pulse.pulse", pulses);
        }
    }
    
    pt.put("count", count);
    
    
    return pt;
}

boost::property_tree::ptree SummarizeHeader(liblas::Header const& header )
{
    using boost::property_tree::ptree;
    ptree pt;
    
    pt.put("filesignature", header.GetFileSignature());
    pt.put("projectdid", header.GetProjectId());
    pt.put("systemid", header.GetSystemId());
    pt.put("softwareid", header.GetSoftwareId());
    
    
    ostringstream version;
    version << static_cast<int>(header.GetVersionMajor());
    version <<".";
    version << static_cast<int>(header.GetVersionMinor());
    pt.put("version", version.str());
    
    pt.put("filesourceid", header.GetFileSourceId());
    pt.put("reserved", header.GetReserved());

#ifdef HAVE_GDAL
    pt.put("srs", header.GetSRS().GetWKT());
#else
#ifdef HAVE_LIBGEOTIFF
    pt.put("proj4", header.GetSRS().GetProj4());
#endif
#endif
    
    ostringstream date;
    date << header.GetCreationDOY() << "/" << header.GetCreationYear();
    pt.put("date", date.str());
    
    pt.put("size", header.GetHeaderSize());
    pt.put("offset", header.GetDataOffset());

    
    pt.put("count", header.GetPointRecordsCount());
    pt.put("dataformatid", header.GetDataFormatId());
    pt.put("datarecordlength", header.GetDataRecordLength());
    
    ptree return_count;
    liblas::Header::RecordsByReturnArray returns = header.GetPointRecordsByReturnCount();
    for (boost::uint32_t i=0; i< 5; i++){
        ptree r;
        r.put("id", i);
        r.put("count", returns[i]);
        return_count.add_child("return", r);
    }
    pt.add_child("returns", return_count);
    
    pt.put("scale.x", header.GetScaleX());
    pt.put("scale.y", header.GetScaleY());
    pt.put("scale.z", header.GetScaleZ());
    
    pt.put("offset.x", header.GetOffsetX());
    pt.put("offset.y", header.GetOffsetY());
    pt.put("offset.z", header.GetOffsetZ());
    
    pt.put("min.x", header.GetMinX());
    pt.put("min.y", header.GetMinY());
    pt.put("min.z", header.GetMinZ());
    
    pt.put("max.x", header.GetMaxX());
    pt.put("max.y", header.GetMaxY());
    pt.put("max.z", header.GetMaxZ());

    
    ptree vlr;
    for (boost::uint32_t i=0; i< header.GetRecordsCount(); i++) {
        liblas::VariableRecord const& r = header.GetVLR(i);
        vlr.put("userid", r.GetUserId(false));
        vlr.put("description", r.GetDescription(false));
        vlr.put("length", r.GetRecordLength());
        vlr.put("id", r.GetRecordId());
        pt.add_child("vlr", vlr);
    }    
    
    return pt;
}

