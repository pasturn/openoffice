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



#pragma once

#include <cppuhelper/compbase8.hxx>

#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/ui/dialogs/XFilePickerNotifier.hpp>
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#include <com/sun/star/ui/dialogs/XFilterGroupManager.hpp>
#include <com/sun/star/ui/dialogs/XFilePickerControlAccess.hpp>
//#include <com/sun/star/ui/dialogs/XFilePreview.hpp>
#include <com/sun/star/util/XCancellable.hpp>

#include <osl/conditn.hxx>
#include <osl/mutex.hxx>

#include <rtl/ustrbuf.hxx>

#include <QString>
#include <QHash>

class KFileDialog;
class QWidget;
class QLayout;

class ResMgr;

class KDE4FilePicker : 
    public cppu::WeakComponentImplHelper8<
        ::com::sun::star::ui::dialogs::XFilterManager,
        ::com::sun::star::ui::dialogs::XFilterGroupManager,
        ::com::sun::star::ui::dialogs::XFilePickerControlAccess,
        ::com::sun::star::ui::dialogs::XFilePickerNotifier,
// TODO ::com::sun::star::ui::dialogs::XFilePreview,
        ::com::sun::star::lang::XInitialization,
        ::com::sun::star::util::XCancellable,
        ::com::sun::star::lang::XEventListener,
        ::com::sun::star::lang::XServiceInfo >		
{
protected:
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xServiceMgr;   // to instanciate own services		

	::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XFilePickerListener > m_xListener;
	
    ResMgr *_resMgr;
	
	//the dialog to display
	KFileDialog* _dialog;
	
	osl::Mutex _helperMutex;
	
	//running filter string to add to dialog
	QString _filter;
	
	//filter for reverse lookup of filter text
	QHash<QString, QString> _filters;
	
	//mapping of SAL control ID's to created custom controls
	QHash<sal_Int16, QWidget*> _customWidgets;
	
	//widget to contain extra custom controls
	QWidget* _extraControls;
	
	//layout for extra custom controls
	QLayout* _layout;

public:
    KDE4FilePicker( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceMgr );
    virtual ~KDE4FilePicker();

    // XFilePickerNotifier

    virtual void SAL_CALL addFilePickerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XFilePickerListener >& xListener ) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL removeFilePickerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XFilePickerListener >& xListener ) throw( ::com::sun::star::uno::RuntimeException );

    // XExecutableDialog functions

    virtual void SAL_CALL setTitle( const ::rtl::OUString &rTitle ) throw( ::com::sun::star::uno::RuntimeException );
    virtual sal_Int16 SAL_CALL execute() throw( ::com::sun::star::uno::RuntimeException );

    // XFilePicker functions

    virtual void SAL_CALL setMultiSelectionMode( sal_Bool bMode ) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL setDefaultName( const ::rtl::OUString &rName ) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL setDisplayDirectory( const ::rtl::OUString &rDirectory ) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::rtl::OUString SAL_CALL getDisplayDirectory() throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getFiles() throw( ::com::sun::star::uno::RuntimeException );

    // XFilterManager functions

    virtual void SAL_CALL appendFilter( const ::rtl::OUString &rTitle, const ::rtl::OUString &rFilter ) throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL setCurrentFilter( const ::rtl::OUString &rTitle ) throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );
    virtual ::rtl::OUString SAL_CALL getCurrentFilter() throw( ::com::sun::star::uno::RuntimeException );

    // XFilterGroupManager functions

    virtual void SAL_CALL appendFilterGroup( const ::rtl::OUString &rGroupTitle, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::StringPair > &rFilters ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);

    // XFilePickerControlAccess functions

    virtual void SAL_CALL setValue( sal_Int16 nControlId, sal_Int16 nControlAction, const ::com::sun::star::uno::Any &rValue ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getValue( sal_Int16 nControlId, sal_Int16 nControlAction ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL enableControl( sal_Int16 nControlId, sal_Bool bEnable ) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL setLabel( sal_Int16 nControlId, const ::rtl::OUString &rLabel ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getLabel( sal_Int16 nControlId ) throw (::com::sun::star::uno::RuntimeException);

    /* TODO XFilePreview

    virtual ::com::sun::star::uno::Sequence< sal_Int16 > SAL_CALL getSupportedImageFormats(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL  getTargetColorDepth(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL  getAvailableWidth(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL  getAvailableHeight(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL       setImage( sal_Int16 aImageFormat, const ::com::sun::star::uno::Any &rImage ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL   setShowState( sal_Bool bShowState ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL   getShowState(  ) throw (::com::sun::star::uno::RuntimeException);
    */

    // XInitialization

    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > &rArguments ) throw( ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException );

    // XCancellable

    virtual void SAL_CALL cancel( ) throw( ::com::sun::star::uno::RuntimeException );

    // XEventListener

    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject &rEvent ) throw( ::com::sun::star::uno::RuntimeException );

    // XServiceInfo

    virtual ::rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString &rServiceName ) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw( ::com::sun::star::uno::RuntimeException );

private:
    // prevent copy and assignment
    KDE4FilePicker( const KDE4FilePicker& );           
    KDE4FilePicker& operator=( const KDE4FilePicker& );
	
	//add a custom control widget to the file dialog
	void addCustomControl(sal_Int16 controlId);
	
}; 
