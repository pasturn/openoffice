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
#include "adabas/BIndexes.hxx"
#include "adabas/BIndex.hxx"
#include "adabas/BTable.hxx"
#include "connectivity/sdbcx/VIndex.hxx"
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbc/XResultSet.hpp>
#include <com/sun/star/sdbc/IndexType.hpp>
#include <comphelper/types.hxx>
#include <comphelper/types.hxx>
#include "adabas/BCatalog.hxx"
#include "connectivity/dbexception.hxx"


using namespace ::comphelper;
using namespace connectivity;
using namespace connectivity::adabas;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
typedef connectivity::sdbcx::OCollection OCollection_TYPE;

sdbcx::ObjectType OIndexes::createObject(const ::rtl::OUString& _rName)
{
	::rtl::OUString aName,aQualifier;
	sal_Int32 nLen = _rName.indexOf('.');
	if(nLen != -1)
	{
		aQualifier	= _rName.copy(0,nLen);
		aName		= _rName.copy(nLen+1);
	}
	else
		aName		= _rName;
	

	Reference< XResultSet > xResult = m_pTable->getMetaData()->getIndexInfo(Any(),
		m_pTable->getSchema(),m_pTable->getTableName(),sal_False,sal_False);

	sdbcx::ObjectType xRet = NULL;
	if(xResult.is())
	{
		Reference< XRow > xRow(xResult,UNO_QUERY);
		while(xResult->next()) 
		{
			if(xRow->getString(6) == aName && (!aQualifier.getLength() || xRow->getString(5) == aQualifier ))
			{
				OAdabasIndex* pRet = new OAdabasIndex(m_pTable,aName,aQualifier,!xRow->getBoolean(4),
					aName == ::rtl::OUString::createFromAscii("SYSPRIMARYKEYINDEX"),
					xRow->getShort(7) == IndexType::CLUSTERED);
				xRet = pRet;
				break;
			}
		}
		::comphelper::disposeComponent(xResult);
	}

	return xRet;
}
// -------------------------------------------------------------------------
void OIndexes::impl_refresh() throw(RuntimeException)
{
	m_pTable->refreshIndexes();
}
// -------------------------------------------------------------------------
Reference< XPropertySet > OIndexes::createDescriptor()
{
	return new OAdabasIndex(m_pTable);
}
// -------------------------------------------------------------------------
// XAppend
sdbcx::ObjectType OIndexes::appendObject( const ::rtl::OUString& _rForName, const Reference< XPropertySet >& descriptor )
{
	if ( m_pTable->isNew() )
		::dbtools::throwFunctionSequenceException(static_cast<XTypeProvider*>(this));

	::rtl::OUString aSql	= ::rtl::OUString::createFromAscii("CREATE ");
	::rtl::OUString aQuote	= m_pTable->getMetaData()->getIdentifierQuoteString(  );
	const ::rtl::OUString& sDot = OAdabasCatalog::getDot();

	if(getBOOL(descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISUNIQUE))))
		aSql = aSql + ::rtl::OUString::createFromAscii("UNIQUE ");
	aSql = aSql + ::rtl::OUString::createFromAscii("INDEX ");

	
	if(_rForName.getLength())
	{
		aSql = aSql + aQuote + _rForName + aQuote
					+ ::rtl::OUString::createFromAscii(" ON ")
					+ aQuote + m_pTable->getSchema() + aQuote + sDot
					+ aQuote + m_pTable->getTableName() + aQuote
					+ ::rtl::OUString::createFromAscii(" ( ");

		Reference<XColumnsSupplier> xColumnSup(descriptor,UNO_QUERY);
		Reference<XIndexAccess> xColumns(xColumnSup->getColumns(),UNO_QUERY);
		Reference< XPropertySet > xColProp;
		sal_Int32 nCount = xColumns->getCount();
		for(sal_Int32 i=0;i<nCount;++i)
		{
			xColumns->getByIndex(i) >>= xColProp;
			aSql = aSql + aQuote + getString(xColProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))) + aQuote;
			aSql = aSql +	(getBOOL(xColProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISASCENDING))) 
										? 
							::rtl::OUString::createFromAscii(" ASC")
										:
							::rtl::OUString::createFromAscii(" DESC"))
						+ 	::rtl::OUString::createFromAscii(",");
		}
		aSql = aSql.replaceAt(aSql.getLength()-1,1,::rtl::OUString::createFromAscii(")"));
	}
	else
	{
		aSql = aSql + aQuote + m_pTable->getSchema() + aQuote + sDot + aQuote + m_pTable->getTableName() + aQuote;

		Reference<XColumnsSupplier> xColumnSup(descriptor,UNO_QUERY);
		Reference<XIndexAccess> xColumns(xColumnSup->getColumns(),UNO_QUERY);
		Reference< XPropertySet > xColProp;
		if(xColumns->getCount() != 1)
			throw SQLException();

		xColumns->getByIndex(0) >>= xColProp;

		aSql = aSql + sDot + aQuote + getString(xColProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))) + aQuote;
	}

	Reference< XStatement > xStmt = m_pTable->getConnection()->createStatement(  );
	xStmt->execute(aSql);
	::comphelper::disposeComponent(xStmt);

    return createObject( _rForName );
}
// -------------------------------------------------------------------------
// XDrop
void OIndexes::dropObject(sal_Int32 /*_nPos*/,const ::rtl::OUString _sElementName)
{
	if(!m_pTable->isNew())
	{
		::rtl::OUString aName,aSchema;
		sal_Int32 nLen = _sElementName.indexOf('.');
		aSchema = _sElementName.copy(0,nLen);
		aName	= _sElementName.copy(nLen+1);

		::rtl::OUString aSql	= ::rtl::OUString::createFromAscii("DROP INDEX ");
		::rtl::OUString aQuote	= m_pTable->getMetaData()->getIdentifierQuoteString(  );
		const ::rtl::OUString& sDot = OAdabasCatalog::getDot();

		if (aSchema.getLength())
			(((aSql += aQuote) += aSchema) += aQuote) += sDot;

		(((aSql += aQuote) += aName) += aQuote) += ::rtl::OUString::createFromAscii(" ON ");

		(((aSql += aQuote) += m_pTable->getSchema()) += aQuote) += sDot;
		((aSql += aQuote) += m_pTable->getTableName()) += aQuote;

		Reference< XStatement > xStmt = m_pTable->getConnection()->createStatement(  );
		xStmt->execute(aSql);
		::comphelper::disposeComponent(xStmt);
	}
}
// -------------------------------------------------------------------------


