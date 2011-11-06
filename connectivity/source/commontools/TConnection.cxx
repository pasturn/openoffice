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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_connectivity.hxx"
#include "TConnection.hxx"
#include <cppuhelper/typeprovider.hxx>
#include <comphelper/types.hxx>
#include <comphelper/officeresourcebundle.hxx>
#include <connectivity/dbexception.hxx>

using namespace connectivity;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::beans;
using namespace ::osl;

//------------------------------------------------------------------------------
OMetaConnection::OMetaConnection() 
    : OMetaConnection_BASE(m_aMutex) 
    , m_nTextEncoding(RTL_TEXTENCODING_MS_1252)
{
}
//------------------------------------------------------------------------------
void OMetaConnection::disposing()
{
	::osl::MutexGuard aGuard(m_aMutex);
    m_xMetaData = WeakReference< XDatabaseMetaData>();
    for (OWeakRefArray::iterator i = m_aStatements.begin(); m_aStatements.end() != i; ++i)
    {
        try 
        {
            Reference< XInterface > xStatement( i->get() );
            ::comphelper::disposeComponent( xStatement );
        }
		catch (DisposedException)
        {
		}
    }
	m_aStatements.clear();
}
//XUnoTunnel
sal_Int64 SAL_CALL OMetaConnection::getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& rId ) throw (::com::sun::star::uno::RuntimeException)
{
	return (rId.getLength() == 16 && 0 == rtl_compareMemory(getUnoTunnelImplementationId().getConstArray(),  rId.getConstArray(), 16 ) )
		? reinterpret_cast< sal_Int64 >( this )
		: (sal_Int64)0;
}
// -----------------------------------------------------------------------------
Sequence< sal_Int8 > OMetaConnection::getUnoTunnelImplementationId()
{
	static ::cppu::OImplementationId * pId = 0;
	if (! pId)
	{
		::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
		if (! pId)
		{
			static ::cppu::OImplementationId aId;
			pId = &aId;
		}
	}
	return pId->getImplementationId();
}
// -----------------------------------------------------------------------------
::dbtools::OPropertyMap& OMetaConnection::getPropMap()
{
	static ::dbtools::OPropertyMap s_aPropertyNameMap;
	return s_aPropertyNameMap;
}
// -----------------------------------------------------------------------------
void OMetaConnection::throwGenericSQLException( sal_uInt16 _nErrorResourceId,const Reference< XInterface>& _xContext )
{
    ::rtl::OUString sErrorMessage;
    if ( _nErrorResourceId )
        sErrorMessage = m_aResources.getResourceString( _nErrorResourceId );
    Reference< XInterface> xContext = _xContext;
    if ( !xContext.is() )
        xContext = *this;
    ::dbtools::throwGenericSQLException( sErrorMessage, xContext);
}

