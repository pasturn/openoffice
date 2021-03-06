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
#include "precompiled_dbui.hxx"

#ifndef _DBAUI_PARAMDIALOG_HXX_
#include "paramdialog.hxx"
#endif
#ifndef _DBAUI_PARAMDIALOG_HRC_
#include "paramdialog.hrc"
#endif
#ifndef _DBU_DLG_HRC_
#include "dbu_dlg.hrc"
#endif
#ifndef _DBAUI_COMMON_TYPES_HXX_
#include "commontypes.hxx"
#endif
#ifndef _DBAUI_MODULE_DBU_HXX_
#include "moduledbu.hxx"
#endif
#ifndef _COM_SUN_STAR_UTIL_XNUMBERFORMATTER_HPP_
#include <com/sun/star/util/XNumberFormatter.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_DATATYPE_HPP_
#include <com/sun/star/sdbc/DataType.hpp>
#endif
#ifndef _CONNECTIVITY_DBTOOLS_HXX_
#include <connectivity/dbtools.hxx>
#endif
#ifndef DBACCESS_SHARED_DBUSTRINGS_HRC
#include "dbustrings.hrc"
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#include <tools/diagnose_ex.h>
#ifndef _DBAUI_LOCALRESACCESS_HXX_
#include "localresaccess.hxx"
#endif
#ifndef INCLUDED_SVTOOLS_SYSLOCALE_HXX
#include <unotools/syslocale.hxx>
#endif

#define EF_VISITED		0x0001
#define EF_DIRTY		0x0002

//.........................................................................
namespace dbaui
{
//.........................................................................
									
	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::lang;
	using namespace ::com::sun::star::beans;
	using namespace ::com::sun::star::container;
	using namespace ::com::sun::star::sdbc;
	using namespace ::com::sun::star::util;
	using namespace ::connectivity;

	//==================================================================
	//= OParameterDialog
	//==================================================================

	//------------------------------------------------------------------------------
	#define INIT_MEMBERS()											\
		:ModalDialog( pParent, ModuleRes(DLG_PARAMETERS))			\
		,m_aNamesFrame	(this, ModuleRes(FL_PARAMS))					\
		,m_aAllParams	(this, ModuleRes(LB_ALLPARAMS))					\
		,m_aValueFrame	(this, ModuleRes(FT_VALUE))						\
		,m_aParam		(this, ModuleRes(ET_PARAM))						\
		,m_aTravelNext	(this, ModuleRes(BT_TRAVELNEXT))				\
		,m_aOKBtn		(this, ModuleRes(BT_OK))						\
		,m_aCancelBtn	(this, ModuleRes(BT_CANCEL))					\
		,m_nCurrentlySelected(LISTBOX_ENTRY_NOTFOUND)				\
		,m_xConnection(_rxConnection)								\
		,m_aPredicateInput( _rxORB, _rxConnection, getParseContext() )	\
		,m_bNeedErrorOnCurrent(sal_True)							\


	//------------------------------------------------------------------------------
DBG_NAME(OParameterDialog)

	OParameterDialog::OParameterDialog(
			Window* pParent, const Reference< XIndexAccess > & rParamContainer,
			const Reference< XConnection > & _rxConnection, const Reference< XMultiServiceFactory >& _rxORB)
		INIT_MEMBERS()
	{
        DBG_CTOR(OParameterDialog,NULL);

		if (_rxORB.is())
			m_xFormatter = Reference< XNumberFormatter>(_rxORB->createInstance(
			::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.util.NumberFormatter"))), UNO_QUERY);
		else {
			DBG_ERROR("OParameterDialog::OParameterDialog: need a service factory!");
        }

		Reference< XNumberFormatsSupplier >  xNumberFormats = ::dbtools::getNumberFormats(m_xConnection, sal_True);
		if (!xNumberFormats.is())
			::comphelper::disposeComponent(m_xFormatter);
		else if (m_xFormatter.is())
			m_xFormatter->attachNumberFormatsSupplier(xNumberFormats);
		try
		{
			DBG_ASSERT(rParamContainer->getCount(), "OParameterDialog::OParameterDialog : can't handle empty containers !");

			m_aFinalValues.realloc(rParamContainer->getCount());
			PropertyValue* pValues = m_aFinalValues.getArray();

			for (sal_Int32 i = 0, nCount = rParamContainer->getCount(); i<nCount; ++i, ++pValues)
			{
				Reference< XPropertySet >  xParamAsSet;
				rParamContainer->getByIndex(i) >>= xParamAsSet;
				OSL_ENSURE(xParamAsSet.is(),"Parameter is null!");
				if(!xParamAsSet.is())
					continue;
				pValues->Name = ::comphelper::getString(xParamAsSet->getPropertyValue(PROPERTY_NAME));
				m_aAllParams.InsertEntry(pValues->Name);

				if (!pValues->Value.hasValue())
					// it won't have a value, 'cause it's default constructed. But may be later we support
					// initializing this dialog with values
					pValues->Value = makeAny(::rtl::OUString());
					// default the values to an empty string

                m_aVisitedParams.push_back(0);
					// not visited, not dirty
			}

			m_xParams = rParamContainer;
		}
		catch(Exception&)
		{
            DBG_UNHANDLED_EXCEPTION();
		}
		

		Construct();

		m_aResetVisitFlag.SetTimeoutHdl(LINK(this, OParameterDialog, OnVisitedTimeout));

		FreeResource();
	}

	//------------------------------------------------------------------------------
	OParameterDialog::~OParameterDialog()
	{
		if (m_aResetVisitFlag.IsActive())
			m_aResetVisitFlag.Stop();

        DBG_DTOR(OParameterDialog,NULL);
    }

	//------------------------------------------------------------------------------
	void OParameterDialog::Construct()
	{
		m_aAllParams.SetSelectHdl(LINK(this, OParameterDialog, OnEntrySelected));
		m_aParam.SetLoseFocusHdl(LINK(this, OParameterDialog, OnValueLoseFocus));
		m_aParam.SetModifyHdl(LINK(this, OParameterDialog, OnValueModified));
		m_aTravelNext.SetClickHdl(LINK(this, OParameterDialog, OnButtonClicked));
		m_aOKBtn.SetClickHdl(LINK(this, OParameterDialog, OnButtonClicked));
		m_aCancelBtn.SetClickHdl(LINK(this, OParameterDialog, OnButtonClicked));

		if (m_aAllParams.GetEntryCount())
		{
			m_aAllParams.SelectEntryPos(0);
			LINK(this, OParameterDialog, OnEntrySelected).Call(&m_aAllParams);

			if (m_aAllParams.GetEntryCount() == 1) 
			{
				m_aTravelNext.Enable(sal_False);
			}

			if (m_aAllParams.GetEntryCount() > 1) 
			{
				m_aOKBtn.SetStyle(m_aOKBtn.GetStyle() & ~WB_DEFBUTTON);
				m_aTravelNext.SetStyle(m_aTravelNext.GetStyle() | WB_DEFBUTTON);
			}
		}

		m_aParam.GrabFocus();
	}

	//------------------------------------------------------------------------------
	IMPL_LINK(OParameterDialog, OnValueLoseFocus, Control*, /*pSource*/)
	{
		if (m_nCurrentlySelected != LISTBOX_ENTRY_NOTFOUND)
		{
			if ( ( m_aVisitedParams[ m_nCurrentlySelected ] & EF_DIRTY ) == 0 )
				// nothing to do, the value isn't dirty
				return 0L;
		}

		// transform the current string according to the param field type
		::rtl::OUString sTransformedText(m_aParam.GetText());
		Reference< XPropertySet >  xParamAsSet;
		m_xParams->getByIndex(m_nCurrentlySelected) >>= xParamAsSet;
		if (xParamAsSet.is())
		{
			if (m_xConnection.is() && m_xFormatter.is())
			{
				::rtl::OUString sParamValue( m_aParam.GetText() );
				sal_Bool bValid = m_aPredicateInput.normalizePredicateString( sParamValue, xParamAsSet );
				m_aParam.SetText( sParamValue );
				if ( bValid )
				{
					// with this the value isn't dirty anymore
					if (m_nCurrentlySelected != LISTBOX_ENTRY_NOTFOUND)
						m_aVisitedParams[m_nCurrentlySelected] &= ~EF_DIRTY;
				}
				else
				{
					if (!m_bNeedErrorOnCurrent)
						return 1L;

					m_bNeedErrorOnCurrent = sal_False;	// will be reset in OnValueModified

					::rtl::OUString sName;
					try 
					{ 
						sName = ::comphelper::getString(xParamAsSet->getPropertyValue(PROPERTY_NAME)); 
					} 
					catch(Exception&) 
					{
                        DBG_UNHANDLED_EXCEPTION();
					}

					String sMessage;
					{
						LocalResourceAccess aDummy(DLG_PARAMETERS, RSC_MODALDIALOG);
						sMessage = String(ModuleRes(STR_COULD_NOT_CONVERT_PARAM));
					}
					sMessage.SearchAndReplaceAll(String::CreateFromAscii("$name$"), sName.getStr());
					ErrorBox(NULL, WB_OK, sMessage).Execute();
					m_aParam.GrabFocus();
					return 1L;
				}
			}
		}

		return 0L;
	}

	//------------------------------------------------------------------------------
	IMPL_LINK(OParameterDialog, OnButtonClicked, PushButton*, pButton)
	{
		if (&m_aCancelBtn == pButton)
		{
			// no interpreting of the given values anymore ....
			m_aParam.SetLoseFocusHdl(Link());	// no direct call from the control anymore ...
			m_bNeedErrorOnCurrent = sal_False;		// in case of any indirect calls -> no error message
			m_aCancelBtn.SetClickHdl(Link());
			m_aCancelBtn.Click();
		}
		else if (&m_aOKBtn == pButton)
		{
			// transfer the current values into the Any
			if (LINK(this, OParameterDialog, OnEntrySelected).Call(&m_aAllParams) != 0L)
			{	// there was an error interpreting the current text
				m_bNeedErrorOnCurrent = sal_True;
					// we're are out of the complex web :) of direct and indirect calls to OnValueLoseFocus now,
					// so the next time it is called we need an error message, again ....
					// (TODO : there surely are better solutions for this ...)
				return 1L;
			}

			if (m_xParams.is())
			{
				// write the parameters
				try
				{
					::rtl::OUString sError;
					PropertyValue* pValues = m_aFinalValues.getArray();
					for (sal_Int32 i = 0, nCount = m_xParams->getCount(); i<nCount; ++i, ++pValues)
					{
						Reference< XPropertySet >  xParamAsSet;
						m_xParams->getByIndex(i) >>= xParamAsSet;

						::rtl::OUString sValue;
						pValues->Value >>= sValue;
						pValues->Value <<= ::rtl::OUString( m_aPredicateInput.getPredicateValue( sValue, xParamAsSet, sal_False ) );
					}
				}
				catch(Exception&)
				{
                    DBG_UNHANDLED_EXCEPTION();
				}
				
			}
			// to close the dialog (which is more code than a simple EndDialog)
			m_aOKBtn.SetClickHdl(Link());
			m_aOKBtn.Click();
		}
		else if (&m_aTravelNext == pButton)
		{
			sal_uInt16 nCurrent = m_aAllParams.GetSelectEntryPos();
			sal_uInt16 nCount = m_aAllParams.GetEntryCount();
			DBG_ASSERT(nCount == m_aVisitedParams.size(), "OParameterDialog::OnButtonClicked : inconsistent lists !");

			// search the next entry in list we haven't visited yet
			sal_uInt16 nNext = (nCurrent + 1) % nCount;
			while ((nNext != nCurrent) && ( m_aVisitedParams[nNext] & EF_VISITED ))
				nNext = (nNext + 1) % nCount;

			if ( m_aVisitedParams[nNext] & EF_VISITED )
				// there is no such "not visited yet" entry -> simpy take the next one
				nNext = (nCurrent + 1) % nCount;

			m_aAllParams.SelectEntryPos(nNext);
			LINK(this, OParameterDialog, OnEntrySelected).Call(&m_aAllParams);
			m_bNeedErrorOnCurrent = sal_True;
				// we're are out of the complex web :) of direct and indirect calls to OnValueLoseFocus now,
				// so the next time it is called we need an error message, again ....
				// (TODO : there surely are better solutions for this ...)
		}

		return 0L;
	}

	//------------------------------------------------------------------------------
	IMPL_LINK(OParameterDialog, OnEntrySelected, ListBox*, /*pList*/)
	{
		if (m_aResetVisitFlag.IsActive())
		{
			LINK(this, OParameterDialog, OnVisitedTimeout).Call(&m_aResetVisitFlag);
			m_aResetVisitFlag.Stop();
		}
		// save the old values
		if (m_nCurrentlySelected != LISTBOX_ENTRY_NOTFOUND)
		{
			// do the transformation of the current text
			if (LINK(this, OParameterDialog, OnValueLoseFocus).Call(&m_aParam) != 0L)
			{	// there was an error interpreting the text
				m_aAllParams.SelectEntryPos(m_nCurrentlySelected);
				return 1L;
			}

			m_aFinalValues[m_nCurrentlySelected].Value <<= ::rtl::OUString(m_aParam.GetText());
		}

		// initialize the controls with the new values
		sal_uInt16 nSelected = m_aAllParams.GetSelectEntryPos();
		DBG_ASSERT(nSelected != LISTBOX_ENTRY_NOTFOUND, "OParameterDialog::OnEntrySelected : no current entry !");

		m_aParam.SetText(::comphelper::getString(m_aFinalValues[nSelected].Value));
		m_nCurrentlySelected = nSelected;

		// with this the value isn't dirty
		DBG_ASSERT(m_nCurrentlySelected < m_aVisitedParams.size(), "OParameterDialog::OnEntrySelected : invalid current entry !");
		m_aVisitedParams[m_nCurrentlySelected] &= ~EF_DIRTY;

		m_aResetVisitFlag.SetTimeout(1000);
		m_aResetVisitFlag.Start();

		return 0L;
	}

	//------------------------------------------------------------------------------
	IMPL_LINK(OParameterDialog, OnVisitedTimeout, Timer*, /*pTimer*/)
	{
		DBG_ASSERT(m_nCurrentlySelected != LISTBOX_ENTRY_NOTFOUND, "OParameterDialog::OnVisitedTimeout : invalid call !");

		// mark the currently selected entry as visited
		DBG_ASSERT(m_nCurrentlySelected < m_aVisitedParams.size(), "OParameterDialog::OnVisitedTimeout : invalid entry !");
		m_aVisitedParams[m_nCurrentlySelected] |= EF_VISITED;

		// was it the last "not visited yet" entry ?
		ConstByteVectorIterator aIter;
		for	(	aIter = m_aVisitedParams.begin();
				aIter < m_aVisitedParams.end();
				++aIter
			)
		{
			if (((*aIter) & EF_VISITED) == 0)
				break;
		}
		if (aIter == m_aVisitedParams.end())
		{	// yes, there isn't another one -> change the "default button"
			m_aTravelNext.SetStyle(m_aTravelNext.GetStyle() & ~WB_DEFBUTTON);
			m_aOKBtn.SetStyle(m_aOKBtn.GetStyle() | WB_DEFBUTTON);
			
			// set to focus to one of the buttons temporary (with this their "default"-state is really updated)
			Window* pOldFocus = Application::GetFocusWindow();

			// if the old focus window is the value edit do some preparations ...
			Selection aSel;
			if (pOldFocus == &m_aParam)
			{
				m_aParam.SetLoseFocusHdl(Link());
				aSel = m_aParam.GetSelection();
			}
			m_aTravelNext.GrabFocus();
			if (pOldFocus)
				pOldFocus->GrabFocus();

			// restore the settings for the value edit
			if (pOldFocus == &m_aParam)
			{
				m_aParam.SetLoseFocusHdl(LINK(this, OParameterDialog, OnValueLoseFocus));
				m_aParam.SetSelection(aSel);
			}
		}

		return 0L;
	}

	//------------------------------------------------------------------------------
	IMPL_LINK(OParameterDialog, OnValueModified, Control*, /*pBox*/)
	{
		// mark the currently selected entry as dirty
		DBG_ASSERT(m_nCurrentlySelected < m_aVisitedParams.size(), "OParameterDialog::OnValueModified : invalid entry !");
		m_aVisitedParams[m_nCurrentlySelected] |= EF_DIRTY;

		m_bNeedErrorOnCurrent = sal_True;

		return 0L;
	}


//.........................................................................
}	// namespace dbaui
//.........................................................................
