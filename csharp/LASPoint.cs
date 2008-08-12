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



    public class LASPoint : IDisposable
    {
        private LASPointH hPoint;
        public void Dispose()
        {

            CAPI.LASPoint_Destroy(hPoint);
            // Clean up unmanaged resources here.
            // Dispose other contained disposable objects.
        }


        enum DataMemberFlag
        {
            eReturnNumber = 1,
            eNumberOfReturns = 2,
            eScanDirection = 4,
            eFlightLineEdge = 8,
            eClassification = 16,
            eScanAngleRank = 32,
            eTime = 64
        };


        enum ClassificationType
        {
            eCreated = 0,
            eUnclassified,
            eGround,
            eLowVegetation,
            eMediumVegetation,
            eHighVegetation,
            eBuilding,
            eLowPoint,
            eModelKeyPoint,
            eWater = 9,
            // = 10 // reserved for ASPRS Definition
            // = 11 // reserved for ASPRS Definition
            eOverlapPoints = 12
            // = 13-31 // reserved for ASPRS Definition
        };

        enum ScanAngleRankRange
        {
            eScanAngleRankMin = -90,
            eScanAngleRankMax = 90
        };



        public LASPointH GetPointer()
        {
            return hPoint;
        }
        public LASPoint(LASPointH hPoint_)
        {
            hPoint = hPoint_;

        }
        //    LASPoint(LASPoint const& other);
        //    LASPoint& operator=(LASPoint const& rhs);

        public double X
        {
            get
            {
                return CAPI.LASPoint_GetX(hPoint);

            }
            set
            {
                LASError error = CAPI.LASPoint_SetX(hPoint, value);
            }
        }

        public double Y
        {
            get
            {
                return CAPI.LASPoint_GetY(hPoint);
            }
            set
            {
                LASError error = CAPI.LASPoint_SetY(hPoint, value);
            }
        }

        public double Z
        {
            get
            {
                return CAPI.LASPoint_GetZ(hPoint);
            }
            set
            {
                LASError error = CAPI.LASPoint_SetZ(hPoint, value);
            }
        }


        //    void SetCoordinates(double const& x, double const& y, double const& z);
        //    void SetCoordinates(LASHeader const& header, double x, double y, double z);

        public UInt16 Intensity
        {
            get
            {
                return CAPI.LASPoint_GetIntensity(hPoint);
            }
            set
            {
                LASError error = CAPI.LASPoint_SetIntensity(hPoint, value);
            }
        }

        public byte ScanFlags
        {
            get
            {
                return CAPI.LASPoint_GetScanFlags(hPoint);
            }
            set
            {
                LASError error = CAPI.LASPoint_SetScanFlags(hPoint, value);
            }
        }

        public UInt16 ReturnNumber
        {
            get
            {
                return CAPI.LASPoint_GetReturnNumber(hPoint);
            }
            set
            {
                LASError error = CAPI.LASPoint_SetReturnNumber(hPoint, value);
            }
        }


        public UInt16 NumberOfReturns
        {
            get
            {
                return CAPI.LASPoint_GetNumberOfReturns(hPoint);
            }
            set
            {
                LASError error = CAPI.LASPoint_SetNumberOfReturns(hPoint, value);
            }
        }



        public UInt16 ScanDirection
        {
            get
            {
                return CAPI.LASPoint_GetScanDirection(hPoint);
            }
            set
            {
                LASError error = CAPI.LASPoint_SetScanDirection(hPoint, value);
            }
        }


        public UInt16 FlightLineEdge
        {
            get
            {
                return CAPI.LASPoint_GetFlightLineEdge(hPoint);
            }
            set
            {
                LASError error = CAPI.LASPoint_SetFlightLineEdge(hPoint, value);
            }
        }

        public byte Classification
        {
            get
            {
                return CAPI.LASPoint_GetClassification(hPoint);
            }
            set
            {
                LASError error = CAPI.LASPoint_SetClassification(hPoint, value);
            }
        }


        public byte ScanAngleRank
        {
            get
            {
                return CAPI.LASPoint_GetScanAngleRank(hPoint);
            }
            set
            {

                LASError error = CAPI.LASPoint_SetScanAngleRank(hPoint, value);
            }
        }

        public byte UserData
        {
            get
            {
                return CAPI.LASPoint_GetUserData(hPoint);
            }
            set
            {
                LASError error = CAPI.LASPoint_SetUserData(hPoint, value);
            }
        }

        public double Time
        {
            get
            {
                return CAPI.LASPoint_GetTime(hPoint);
            }
            set
            {
                LASError error = CAPI.LASPoint_SetTime(hPoint, value);
            }
        }








        //    double& operator[](std::size_t const& n);
        //    double const& operator[](std::size_t const& n) const;

        //    bool equal(LASPoint const& other) const;

        //    bool Validate() const;
        //    bool IsValid() const;

        //private:

        // static std::size_t const coords_size = 3;
        // double m_coords[coords_size];
        //Uint16 m_intensity;
        //byte m_flags;
        //byte m_class;
        //byte m_angleRank;
        //byte m_userData;
        //double m_gpsTime;

        //    void throw_out_of_range() const
        //    {
        //        throw std::out_of_range("coordinate subscript out of range");
        //    }
        //};


    }
}
