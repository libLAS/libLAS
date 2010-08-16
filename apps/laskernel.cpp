
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

    std::vector<uint16_t> returns;
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

    std::vector<uint8_t> classes;
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
    liblas::ContinuousValueFilter<uint16_t>::filter_func f = &liblas::Point::GetIntensity;
    liblas::ContinuousValueFilter<uint16_t>* intensity_filter = new liblas::ContinuousValueFilter<uint16_t>(f, intensities);
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
    liblas::ContinuousValueFilter<int8_t>::filter_func f = &liblas::Point::GetScanAngleRank;
    liblas::ContinuousValueFilter<int8_t>* intensity_filter = new liblas::ContinuousValueFilter<int8_t>(f, intensities);
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
    ("thin,t", po::value<uint32_t>()->default_value(0), "Simple decimation-style thinning.\nThin the file by removing every t'th point from the file.")
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
                rgb.push_back(atof((*c).c_str()));
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
                rgb.push_back(atof((*c).c_str()));
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
        header.SetVersionMinor(static_cast<uint8_t>(minor)); 
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
