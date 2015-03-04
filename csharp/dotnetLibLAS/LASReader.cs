/******************************************************************************
 * $Id$
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
using System.IO;

namespace LibLAS
{
    /// <summary>
    /// LASReader class
    /// </summary>
    public class LASReader : IDisposable
    {
        private LASReaderH hReader;
        private LASPointH hPoint;
        private LASPoint point;

        /// <summary>
        /// Creates a LASReaderH object that can be used to read LASHeaderH and LASPointH objects with.
        /// </summary>
        /// <remarks>The LASReaderH must not be created with a filename that is opened for read or write by any other API functions. </remarks>
        /// <param name="filename">filename to open for read</param>
        public LASReader(String filename)
        {
            // If file does not exist LASReader_Create(filename) throws a 
            // "System.AccessViolationException: Attempted to read or write protected memory".
            // This type of exception cannot be caught because it indicates a corrupted program state.
            // So, check file existence up front.
            if (!File.Exists(filename))
            {
                throw new FileNotFoundException("LASReader could not find the specified file", filename);
            }

            hReader = NativeMethods.LASReader_Create(filename);
        }

        /// <summary>
        /// Reads the next available point on the LASReaderH instance. 
        /// </summary>
        /// <returns>true if we have next point</returns>
        public bool GetNextPoint()
        {
            IntPtr tmphPoint = NativeMethods.LASReader_GetNextPoint(hReader);

            if (IntPtr.Zero != tmphPoint)
            {
                hPoint = tmphPoint;
                point = new LASPoint(hPoint, false);
                return true;
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public string GetVersion()
        {
            return NativeMethods.LAS_GetVersion();
        }
       
        /// <summary>
        /// get the current LASPoint.
        /// </summary>
        /// <returns>current LASPoint object</returns>
        public LASPoint GetPoint()
        {
            return point;
        }

        /// <summary>
        /// Reads a LASPointH from the given position in the LAS file represented by the LASReader instance.
        /// </summary>
        /// <remarks> If no point is available at that location, NULL is returned. </remarks>
        /// <param name="position">the integer position of the point in the file to read.</param>
        /// <returns>LASPoint object</returns>
        public LASPoint GetPointAt(UInt32 position)
        {
            IntPtr tmphPoint = NativeMethods.LASReader_GetPointAt(hReader, position);
            if (IntPtr.Zero != tmphPoint)
            {
                hPoint = tmphPoint;
                return new LASPoint(hPoint, false);
            }
            return null;
        }

        /// <summary>
        /// Get the header for the file associated with this Reader Class.
        /// </summary>
        /// <returns>LASHeader representing the header for the file.</returns>
        public LASHeader GetHeader()
        {
            return new LASHeader(NativeMethods.LASReader_GetHeader(hReader));
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                // free managed resources
                if (point != null)
                {
                    point.Dispose();
                    point = null;
                }
            }

            // free native resources if there are any.
            if (hReader != IntPtr.Zero)
            {
                NativeMethods.LASReader_Destroy(hReader);
                hReader = IntPtr.Zero;
            }
            if(hPoint != IntPtr.Zero)
            {
                NativeMethods.LASPoint_Destroy(hPoint);
                hPoint = IntPtr.Zero;
            }
        }

        /// <summary>
        /// The object user should call this method when they finished with the object.
        /// </summary>
        /// 
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        ~LASReader()
        {
            // Finalizer calls Dispose(false)
            Dispose(false);
        }
    }
}
