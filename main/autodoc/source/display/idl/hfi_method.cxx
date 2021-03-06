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

#include <precomp.h>
#include "hfi_method.hxx"


// NOT FULLY DEFINED SERVICES
#include <ary/idl/i_exception.hxx>
#include <ary/idl/i_param.hxx>
#include <toolkit/hf_docentry.hxx>
#include <toolkit/hf_funcdecl.hxx>
#include "hfi_doc.hxx"
#include "hfi_globalindex.hxx"
#include "hfi_typetext.hxx"





HF_IdlMethod::HF_IdlMethod( Environment &           io_rEnv,
                            Xml::Element &          o_cell)
    :   HtmlFactory_Idl(io_rEnv,&o_cell)
{
}


HF_IdlMethod::~HF_IdlMethod()
{
}


void
HF_IdlMethod::Produce_byData( const String &      i_sName,
                              type_id             i_nReturnType,
                              param_list &        i_rParams,
                              type_list &         i_rExceptions,
                              bool                i_bOneway,
                              bool                i_bEllipse,
                              const client &      i_ce ) const
{
    CurOut()
        >> *new Html::Label(i_sName)
            << new Html::ClassAttr(C_sMemberTitle)
            << i_sName;
    enter_ContentCell();
    write_Declaration( i_sName,
                       i_nReturnType,
                       i_rParams,
                       i_rExceptions,
                       i_bOneway,
                       i_bEllipse );
    CurOut() << new Html::HorizontalLine;
    write_Docu(CurOut(), i_ce);
    leave_ContentCell();
}

#if 0 // old
void
HF_IdlMethod::write_Declaration( const String &      i_sName,
                                 type_id             i_nReturnType,
                                 param_list &        i_rParams,
                                 type_list &         i_rExceptions,
                                 bool                i_bOneway,
                                 bool				 i_bEllipse ) const
{
    HF_FunctionDeclaration
        aDecl(CurOut()) ;
    Xml::Element &
        front = aDecl.Add_ReturnLine();

    // Front:
    if (i_bOneway)
        front << "[oneway] ";
    if (i_nReturnType.IsValid())
    {   // Normal function, but not constructors:
        HF_IdlTypeText
            aReturn(Env(), front, true);
        aReturn.Produce_byData(i_nReturnType);
        front
            << new Html::LineBreak;

    }
    front
        >> *new Html::Bold
            << i_sName;

    //  Main line:
    Xml::Element &
        types = aDecl.Types();
    Xml::Element &
        names = aDecl.Names();
    bool bParams = i_rParams.operator bool();
    if (bParams)
    {
        front
            << "(";
        HF_IdlTypeText
            aType( Env(), types, true );

        write_Param( aType, names, (*i_rParams) );

        for (++i_rParams; i_rParams; ++i_rParams)
        {
            types
                << new Html::LineBreak;
            names
                << ","
                << new Html::LineBreak;
            write_Param( aType, names, (*i_rParams) );
        }   // end for

        if (i_bEllipse)
        {
            names
                << " ...";
        }
        names
            << " )";
    }
    else
        front
            << "()";


    if ( i_rExceptions.operator bool() )
    {
        Xml::Element &
            rExcOut = aDecl.Add_RaisesLine("raises", NOT bParams);
        HF_IdlTypeText
            aExc(Env(), rExcOut, true);
        aExc.Produce_byData(*i_rExceptions);

        for (++i_rExceptions; i_rExceptions; ++i_rExceptions)
        {
            rExcOut
                << ","
                << new Html::LineBreak;
            aExc.Produce_byData(*i_rExceptions);
        }   // end for

        rExcOut << " );";
    }
    else
    {
        if (bParams)
            aDecl.Names() << ";";
        else
            aDecl.Front() << ";";
    }
}
#endif   // 0  old

void
HF_IdlMethod::write_Declaration( const String &      i_sName,
                                 type_id             i_nReturnType,
                                 param_list &        i_rParams,
                                 type_list &         i_rExceptions,
                                 bool                i_bOneway,
                                 bool				 i_bEllipse ) const
{
    HF_FunctionDeclaration
        aDecl(CurOut(), "raises") ;
    Xml::Element &
        rReturnLine = aDecl.ReturnCell();

    // Return line:
    if (i_bOneway)
        rReturnLine << "[oneway] ";
    if (i_nReturnType.IsValid())
    {   // Normal function, but not constructors:
        HF_IdlTypeText
            aReturn(Env(), rReturnLine, true);
        aReturn.Produce_byData(i_nReturnType);
    }

    //  Main line:
    Xml::Element &
        rNameCell = aDecl.NameCell();
    rNameCell
        >> *new Html::Bold
            << i_sName;

    Xml::Element *
        pParamEnd  = 0;

    bool bParams = i_rParams.operator bool();
    if (bParams)
    {
        rNameCell
            << "(";

        pParamEnd  = write_Param( aDecl, *i_rParams );
        for (++i_rParams; i_rParams; ++i_rParams)
        {
            *pParamEnd << ",";
            pParamEnd  = write_Param( aDecl, *i_rParams );
        }   // end for

        if (i_bEllipse)
        {
            Xml::Element &
                rParamType = aDecl.NewParamTypeCell();
            rParamType
                << " ...";
            pParamEnd = &rParamType;
        }
        *pParamEnd
            << " )";
    }
    else
    {
        rNameCell
            << "()";
    }

    if ( i_rExceptions.operator bool() )
    {
        Xml::Element &
            rExcOut = aDecl.ExceptionCell();
        HF_IdlTypeText
            aExc(Env(), rExcOut, true);
        aExc.Produce_byData(*i_rExceptions);

        for (++i_rExceptions; i_rExceptions; ++i_rExceptions)
        {
            rExcOut
                << ","
                << new Html::LineBreak;
            aExc.Produce_byData(*i_rExceptions);
        }   // end for

        rExcOut << " );";
    }
    else if (bParams)
    {
        *pParamEnd << ";";
    }
    else
    {
        rNameCell << ";";
    }
}

#if 0   // old
void
HF_IdlMethod::write_Param( HF_IdlTypeText &             o_type,
                           Xml::Element &               o_names,
                           const ary::idl::Parameter &  i_param ) const
{
    switch ( i_param.Direction() )
    {
        case ary::idl::param_in:
                    o_type.CurOut() << "[in] ";
                    break;
        case ary::idl::param_out:
                    o_type.CurOut() << "[out] ";
                    break;
        case ary::idl::param_inout:
                    o_type.CurOut() << "[inout] ";
                    break;
    }   // end switch

    o_type.Produce_byData( i_param.Type() );
    o_names
        << i_param.Name();
}
#endif  // 0   old

Xml::Element *
HF_IdlMethod::write_Param( HF_FunctionDeclaration &     o_decl,
                           const ary::idl::Parameter &  i_param ) const
{
    Xml::Element &
        rTypeCell  = o_decl.NewParamTypeCell();
    Xml::Element &
        rNameCell  = o_decl.ParamNameCell();

    switch ( i_param.Direction() )
    {
        case ary::idl::param_in:
                    rTypeCell << "[in] ";
                    break;
        case ary::idl::param_out:
                    rTypeCell << "[out] ";
                    break;
        case ary::idl::param_inout:
                    rTypeCell << "[inout] ";
                    break;
    }   // end switch

    HF_IdlTypeText
        aTypeWriter(Env(), rTypeCell, true);
    aTypeWriter.Produce_byData( i_param.Type() );

    rNameCell
        << i_param.Name();
    return &rNameCell;
}

const String sContentBorder("0");
const String sContentWidth("96%");
const String sContentPadding("5");
const String sContentSpacing("0");

const String sBgWhite("#ffffff");
const String sCenter("center");

void
HF_IdlMethod::enter_ContentCell() const
{

    Xml::Element &
        rContentCell = CurOut()
                        >> *new Html::Table( sContentBorder,
                                             sContentWidth,
                                             sContentPadding,
                                             sContentSpacing )
                            << new Html::ClassAttr("table-in-method")
                            << new Html::BgColorAttr(sBgWhite)
                            << new Html::AlignAttr(sCenter)
                            >> *new Html::TableRow
                                >> *new Html::TableCell;
    Out().Enter(rContentCell);
}


void
HF_IdlMethod::leave_ContentCell() const
{
    Out().Leave();
}

