/******************************************************************************
 * 
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  
 * Author:   Martin Vales, martin_gnu@mundo-r.com
 *
 ******************************************************************************
 * Copyright (c) 2008, Martin Vales
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following 
 * conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided 
 *       with the distribution.
 *     * Neither the name of the Martin Isenburg or Iowa Department 
 *       of Natural Resources nor the names of its contributors may be 
 *       used to endorse or promote products derived from this software 
 *       without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 ****************************************************************************/



using System;

using System.Collections.Generic;

using System.Text;

using LASError = System.Int32;

using LASWriterH = System.IntPtr;

using LASReaderH = System.IntPtr;

using LASPointH = System.IntPtr;

using LASGuidH = System.IntPtr;

using LASVLRH = System.IntPtr;

using LASHeaderH = System.IntPtr;



namespace LibLAS
{

    public class LASHeader : IDisposable
    {



        // The object user should call this method

        // when they finished with the object.

        public void Dispose()
        {



            CAPI.LASHeader_Destroy(hHeader);

            // Clean up unmanaged resources here.

            // Dispose other contained disposable objects.

        }







        enum FormatVersion
        {

            eVersionMajorMin = 1, ///< Minimum of major component

            eVersionMajorMax = 1, ///< Maximum of major component

            eVersionMinorMin = 0, ///< Minimum of minor component

            eVersionMinorMax = 1 ///< Maximum of minor component

        };



        /// Versions of point record format.

        enum PointFormat
        {

            ePointFormat0 = 0, ///< Point Data Format \e 0

            ePointFormat1 = 1  ///< Point Data Format \e 1

        };



        /// Number of bytes of point record storage in particular format.

        enum PointSize
        {

            ePointSize0 = 20, ///< Size of point record in data format \e 0

            ePointSize1 = 28  ///< Size of point record in data format \e 1

        };





        // Official signature of ASPRS LAS file format, always \b "LASF".

        public static readonly string FileSignature_;



        // Default system identifier used by libLAS, always \b "libLAS".

        public static readonly string SystemIdentifier;



        //Default software identifier used by libLAS, always \b "libLAS X.Y".

        public static readonly string SoftwareIdentifier;





        private LASHeaderH hHeader;



        public LASHeaderH GetPointer()
        {

            return hHeader;

        }



        //Default constructor.

        //The default constructed header is configured according to the ASPRS

        //LAS 1.1 Specification, point data format set to 0.

        //Other fields filled with 0.

        public LASHeader(LASHeaderH hHeader_)
        {

            hHeader = hHeader_;

        }



        public LASHeader()
        {

            hHeader = CAPI.LASHeader_Create();

        }



        public LASHeader Copy()
        {

            return new LASHeader(CAPI.LASHeader_Copy(hHeader));

        }



        public void Destroy()
        {

            CAPI.LASHeader_Destroy(hHeader);

        }



        //    /// Assignment operator.

        //    LASHeader& operator=(LASHeader const& rhs);



        //    /// Comparison operator.

        //    bool operator==(const LASHeader& other) const;









        //Set ASPRS LAS file signature.

        //The only value allowed as file signature is \b "LASF",

        //defined as FileSignature constant.

        //\exception std::invalid_argument - if invalid signature given.

        //\param v - string contains file signature, at least 4-bytes long

        //with "LASF" as first four bytes.



        //Get ASPRS LAS file signature.

        //\return 4-characters long string - \b "LASF".

        public string FileSignature
        {

            get
            {

                return CAPI.LASHeader_GetFileSignature(hHeader);

            }

        }



        //Get file source identifier.

        //\exception No throw

        //    /// Set file source identifier.

        //    /// \param v - should be set to a value between 1 and 65535.

        //    /// \exception No throw

        public UInt16 FileSourceId
        {

            get
            {

                return CAPI.LASHeader_GetFileSourceId(hHeader);

            }



        }







        //    /// Get value field reserved by the ASPRS LAS Specification.

        //    /// \note This field is always filled with 0.



        public Int16 Reserved
        {

            get
            {

                return CAPI.LASHeader_GetReserved(hHeader);

            }

            //set

            //{



            //}

        }





        //Get project identifier.

        //\return Global Unique Identifier as an instance of liblas::guid class.

        public String ProjectId
        {

            get
            {

                return CAPI.LASHeader_GetProjectId(hHeader);

            }

            //set

            //{

            //    //    void SetProjectId(guid const& v);

            //    //status = value;

            //}

        }









        public byte VersionMajor
        {

            get
            {

                //Get major component of version of LAS format.

                //\return Always 1 is returned as the only valid value.

                return CAPI.LASHeader_GetVersionMajor(hHeader);

            }

            set
            {



                //Set major component of version of LAS format.

                //\exception std::out_of_range - invalid value given.

                //\param v - value between eVersionMajorMin and eVersionMajorMax.

                LASError error = CAPI.LASHeader_SetVersionMajor(hHeader, value);

                if ((Int32)error != 0)
                {
                    LASException e = new LASException("Exception in Set Header VersionMajor.");

                    throw e;
                }



            }

        }







        public byte VersionMinor
        {

            get
            {

                //Get minor component of version of LAS format.

                //\return Valid values are 1 or 0.

                return CAPI.LASHeader_GetVersionMinor(hHeader);

            }

            set
            {

                //Set minor component of version of LAS format.

                //\exception std::out_of_range - invalid value given.

                //\param v - value between eVersionMinorMin and eVersionMinorMax.

                LASError error = CAPI.LASHeader_SetVersionMinor(hHeader, value);

                if ((Int32)error != 0)
                {
                    LASException e = new LASException("Exception in Set Header VersionMinor.");

                    throw e;
                }

            }

        }



        public String SystemId
        {

            get
            {

                //Get system identifier.

                //Default value is \b "libLAS" specified as the SystemIdentifier constant.

                //\param pad - if true the returned string is padded right with spaces and

                //its length is 32 bytes, if false (default) no padding occurs and

                //length of the returned string is <= 32 bytes.

                //\return value of system identifier field.

                return CAPI.LASHeader_GetSystemId(hHeader);

            }

            set
            {

                //Set system identifier.

                //\exception std::invalid_argument - if identifier longer than 32 bytes.

                //\param v - system identifiers string.

                LASError error = CAPI.LASHeader_SetSystemId(hHeader, value);

                if ((Int32)error != 0)
                {
                    LASException e = new LASException("Exception in Set Header SystemId.");

                    throw e;
                }


            }

        }







        public String SoftwareId
        {

            get
            {

                //Get software identifier.

                //Default value is \b "libLAS 1.0", specified as the SoftwareIdentifier constant.

                //\param pad - if true the returned string is padded right with spaces and its length is 32 bytes,

                //if false (default) no padding occurs and length of the returned string is <= 32 bytes.

                //\return value of generating software identifier field.

                return CAPI.LASHeader_GetSoftwareId(hHeader);

            }

            set
            {

                //Set software identifier.

                //\exception std::invalid_argument - if identifier is longer than 32 bytes.

                //\param v - software identifiers string.

                LASError error = CAPI.LASHeader_SetSoftwareId(hHeader, value);
                if ((Int32)error != 0)
                {
                    LASException e = new LASException("Exception in Set Header SoftwareId.");

                    throw e;
                }


            }

        }













        public UInt16 CreationDOY
        {

            get
            {

                //Get day of year of file creation date.

                //\todo Use full date structure instead of Julian date number.

                return CAPI.LASHeader_GetCreationDOY(hHeader);

            }

            set
            {

                //Set day of year of file creation date.

                //\exception std::out_of_range - given value is higher than number 366.

                //\todo Use full date structure instead of Julian date number.

                LASError error = CAPI.LASHeader_SetCreationDOY(hHeader, value);
                if ((Int32)error != 0)
                {
                    LASException e = new LASException("Exception in Set Header CreationDOY.");

                    throw e;
                }


            }

        }













        public UInt16 CreationYear
        {

            get
            {

                //Get year of file creation date.

                return CAPI.LASHeader_GetCreationYear(hHeader);

            }

            set
            {

                // Set year of file creation date.

                //\exception std::out_of_range - given value is higher than number 9999.

                //\todo Remove if full date structure is used.

                LASError error = CAPI.LASHeader_SetCreationYear(hHeader, value);

                if ((Int32)error != 0)
                {
                    LASException e = new LASException("Exception in Set Header CreationYear.");

                    throw e;
                }

            }

        }













        public UInt16 HeaderSize
        {

            get
            {

                //Get number of bytes of generic verion of public header block storage.

                //Standard version of the public header block is 227 bytes long.

                return CAPI.LASHeader_GetHeaderSize(hHeader);

            }

        }



        public UInt32 DataOffset
        {

            get
            {



                //Get number of bytes from the beginning to the first point record.

                return CAPI.LASHeader_GetDataOffset(hHeader);

            }

            //set

            //{

            //    //    void SetDataOffset(uint32_t v);

            //    //status = value;

            //    /// Set number of bytes from the beginning to the first point record.

            //    /// \exception std::out_of_range - if given offset is bigger than 227+2 bytes

            //    /// for the LAS 1.0 format and 227 bytes for the LAS 1.1 format.

            //}

        }





        public UInt32 RecordsCount
        {

            get
            {

                return CAPI.LASHeader_GetRecordsCount(hHeader);

            }

            //set

            //{

            //    //    void SetRecordsCount(uint32_t v);

            //    //status = value;

            //}

        }





        public byte DataFormatId
        {

            get
            {

                return CAPI.LASHeader_GetDataFormatId(hHeader);

            }

            set
            {

                LASError error = CAPI.LASHeader_SetDataFormatId(hHeader, value);
                if ((Int32)error != 0)
                {
                    LASException e = new LASException("Exception in Set Header DataFormatId.");

                    throw e;
                }

            }

        }









        public UInt16 DataRecordLength
        {

            get
            {



                return CAPI.LASHeader_GetDataRecordLength(hHeader);

            }



            //set

            //{

            //    

            //}

        }







        public UInt32 PointRecordsCount
        {

            get
            {

                //Get total number of point records stored in the LAS file.

                return CAPI.LASHeader_GetPointRecordsCount(hHeader);

            }

            set
            {

                //Set number of point records that will be stored in a new LAS file.

                LASError error = CAPI.LASHeader_SetPointRecordsCount(hHeader, value);
                if ((Int32)error != 0)
                {
                    LASException e = new LASException("Exception in Set Header PointRecordsCount.");

                    throw e;
                }

            }

        }







        public UInt32 GetPointRecordsByReturnCount(int index)
        {

            // Get array of the total point records per return.

            return CAPI.LASHeader_GetPointRecordsByReturnCount(hHeader, index);

        }



        //    /// Set values of 5-elements array of total point records per return.

        //    /// \exception std::out_of_range - if index is bigger than 4.

        //    /// \param index - subscript (0-4) of array element being updated.

        //    /// \param v - new value to assign to array element identified by index.

        public void SetPointRecordsByReturnCount(int index, UInt32 value)
        {

            LASError error = CAPI.LASHeader_SetPointRecordsByReturnCount(hHeader, index, value);
            if ((Int32)error != 0)
            {
                LASException e = new LASException("Exception in Set Header SetPointRecordsByReturnCount.");

                throw e;
            }

        }



        // Get scale factor for X coordinate.

        public double GetScaleX()
        {

            return CAPI.LASHeader_GetScaleX(hHeader);

        }





        //Get scale factor for Y coordinate.

        public double GetScaleY()
        {

            return CAPI.LASHeader_GetScaleY(hHeader);



        }



        // Get scale factor for Z coordinate.

        public double GetScaleZ()
        {

            return CAPI.LASHeader_GetScaleZ(hHeader);

        }





        //Set values of scale factor for X, Y and Z coordinates.

        public void SetScale(double x, double y, double z)
        {

            LASError error = CAPI.LASHeader_SetScale(hHeader, x, y, z);
            if ((Int32)error != 0)
            {
                LASException e = new LASException("Exception in Set Header SetScale.");

                throw e;
            }

        }





        //Get X coordinate offset.

        public double GetOffsetX()
        {

            return CAPI.LASHeader_GetOffsetX(hHeader);



        }





        //Get Y coordinate offset.

        public double GetOffsetY()
        {

            return CAPI.LASHeader_GetOffsetY(hHeader);

        }





        //Get Z coordinate offset.

        public double GetOffsetZ()
        {

            return CAPI.LASHeader_GetOffsetZ(hHeader);

        }





        //Set values of X, Y and Z coordinates offset.

        public void SetOffset(double x, double y, double z)
        {

            LASError error = CAPI.LASHeader_SetOffset(hHeader, x, y, z);

            if ((Int32)error != 0)
            {
                LASException e = new LASException("Exception in Set Header SetOffset.");

                throw e;
            }

        }





        //Get maximum value of extent of X coordinate.

        public double MaxX()
        {

            return CAPI.LASHeader_GetMaxX(hHeader);

        }



        //Get maximum value of extent of Y coordinate.

        public double GetMaxY()
        {

            return CAPI.LASHeader_GetMaxY(hHeader);

        }



        //Get maximum value of extent of Z coordinate.

        public double GetMaxZ()
        {

            return CAPI.LASHeader_GetMaxZ(hHeader);



        }



        //Get minimum value of extent of X coordinate.

        public double GetMinX()
        {



            return CAPI.LASHeader_GetMinX(hHeader);





        }



        //Get minimum value of extent of Y coordinate.

        public double GetMinY()
        {



            return CAPI.LASHeader_GetMinY(hHeader);





        }

        //Get minimum value of extent of Z coordinate.

        public double GetMinZ()
        {



            return CAPI.LASHeader_GetMinZ(hHeader);



        }



        //Set maximum values of extent of X, Y and Z coordinates.

        public void SetMax(double x, double y, double z)
        {

            LASError error = CAPI.LASHeader_SetMax(hHeader, x, y, z);

            if ((Int32)error != 0)
            {
                LASException e = new LASException("Exception in Set Header SetMax.");

                throw e;
            }

        }



        //Set minimum values of extent of X, Y and Z coordinates.

        public void SetMin(double x, double y, double z)
        {

            LASError error = CAPI.LASHeader_SetMin(hHeader, x, y, z);
            if ((Int32)error != 0)
            {
                LASException e = new LASException("Exception in Set Header SetMin.");

                throw e;
            }

        }

















        //    /// Adds a variable length record to the header

        //    void AddVLR(LASVLR const& v);



        //    /// Returns a VLR 

        //    LASVLR const& GetVLR(uint32_t index) const;



        //    /// Removes a VLR from the the header

        //    void DeleteVLR(uint32_t index);





        public string Proj4
        {

            get
            {

                return CAPI.LASHeader_GetProj4(hHeader);

            }

            set
            {

                LASError error = CAPI.LASHeader_SetProj4(hHeader, value);
                if ((Int32)error != 0)
                {
                    LASException e = new LASException("Exception in Set Header Proj4.");

                    throw e;
                }

            }

        }



        //public string Georeference

        //{

        //    get

        //    {



        //        return "ss";

        //    }

        //    set

        //    {

        //        //    void SetGeoreference();



        //        //status = value;

        //    }

        //}



        //    /// Fetch the Georeference as a proj.4 string





        //    /// Set the Georeference as a proj.4 string









        //private:



        //    typedef detail::Point<double> PointScales;

        //    typedef detail::Point<double> PointOffsets;

        //    typedef detail::Extents<double> PointExtents;



        //    enum

        //    {

        //        eDataSignatureSize = 2,

        //        eFileSignatureSize = 4,

        //        ePointsByReturnSize = 5,

        //        eProjectId4Size = 8,

        //        eSystemIdSize = 32,

        //        eSoftwareIdSize = 32,

        //        eHeaderSize = 227,

        //        eFileSourceIdMax = 65535

        //    };



        //    // TODO (low-priority): replace static-size char arrays

        //    // with std::string and return const-reference to string object.



        //    //

        //    // Private function members

        //    //

        //    void Init();

        //    void ClearGeoKeyVLRs();



        //    //

        //    // Private data members

        //    //

        //    char m_signature[eFileSignatureSize];

        //    uint16_t m_sourceId;

        //    uint16_t m_reserved;

        //    uint32_t m_projectId1;

        //    uint16_t m_projectId2;

        //    uint16_t m_projectId3;

        //    uint8_t m_projectId4[eProjectId4Size];

        //    uint8_t m_versionMajor;

        //    uint8_t m_versionMinor;

        //    char m_systemId[eSystemIdSize];

        //    char m_softwareId[eSoftwareIdSize];

        //    uint16_t m_createDOY;

        //    uint16_t m_createYear;

        //    uint16_t m_headerSize;

        //    uint32_t m_dataOffset;

        //    uint32_t m_recordsCount;

        //    uint8_t m_dataFormatId;

        //    uint16_t m_dataRecordLen;

        //    uint32_t m_pointRecordsCount;

        //    std::vector<uint32_t> m_pointRecordsByReturn;

        //    PointScales m_scales;

        //    PointOffsets m_offsets;

        //    PointExtents m_extents;

        //    std::vector<LASVLR> m_vlrs;

        //    std::string m_proj4;

    }

}

