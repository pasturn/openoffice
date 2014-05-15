/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifdef _MSC_VER
#pragma warning (disable : 4800)
#endif

#ifndef _PATH_HELPER_HXX_
#define _PATH_HELPER_HXX_

#include "path_helper.h"
#include <rtl/ustring.hxx>

namespace osl
{

/*******************************************************************
 osl_systemPathEnsureSeparator
 Adds a trailing path separator to the given system path if not 
 already there and if the path is not the root path or a logical
 drive alone
 ******************************************************************/
 
inline void systemPathEnsureSeparator(/*inout*/ rtl::OUString& Path)
{
    osl_systemPathEnsureSeparator(&Path.pData);
}

/*******************************************************************
 osl_systemPathRemoveSeparator
 Removes the last separator from the given system path if any and 
 if the path is not the root path '\'
 ******************************************************************/
 
inline void systemPathRemoveSeparator(/*inout*/ rtl::OUString& Path)
{
    osl_systemPathRemoveSeparator(&Path.pData);
}

/*******************************************************************
 osl_systemPathIsLogicalDrivePattern 
 ******************************************************************/
 
inline bool systemPathIsLogicalDrivePattern(/*in*/ const rtl::OUString& path)
{
    return osl_systemPathIsLogicalDrivePattern(path.pData);
}
 
/*******************************************************************
 LongPathBuffer
 ******************************************************************/
template< class T >
class LongPathBuffer
{
    T* m_pBuffer;
    sal_uInt32 m_nCharNum;

    LongPathBuffer();
    LongPathBuffer( const LongPathBuffer& );
    LongPathBuffer& operator=( const LongPathBuffer& );

public:
    LongPathBuffer( sal_uInt32 nCharNum )
    : m_pBuffer( reinterpret_cast<T*>( rtl_allocateMemory( nCharNum * sizeof( T ) ) ) )
    , m_nCharNum( nCharNum )
    {
        OSL_ENSURE( m_pBuffer, "Can not allocate the buffer!" );
    }

    ~LongPathBuffer()
    {
        if ( m_pBuffer )
            rtl_freeMemory( m_pBuffer );
        m_pBuffer = 0;
    }

    sal_uInt32 getBufSizeInSymbols()
    {
        return m_nCharNum;
    }

    operator T* ()
    {
        return m_pBuffer;
    }

};

    template< class U, class T > U mingw_reinterpret_cast(LongPathBuffer<T>& a) { return reinterpret_cast<U>(static_cast<T*>(a)); }

} // end namespace osl

#endif 
