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


#ifndef _SVX_UNOMID_HXX
#define _SVX_UNOMID_HXX

//AdjustItem
#define MID_PARA_ADJUST			0
#define MID_LAST_LINE_ADJUST    1
#define MID_EXPAND_SINGLE       2

//SvxPageItem
#define MID_PAGE_NUMTYPE		0
#define MID_PAGE_ORIENTATION    1
#define MID_PAGE_LAYOUT         2

//SvxMarginItem
#define MID_MARGIN_LR_MARGIN	0
#define MID_MARGIN_UL_MARGIN	1
#define MID_MARGIN_L_MARGIN		2
#define MID_MARGIN_R_MARGIN		3
#define MID_MARGIN_UP_MARGIN	4
#define MID_MARGIN_LO_MARGIN	5

#define ALL_BORDER					10
#define ALL_BORDER_PADDING			11
#define LEFT_BORDER_PADDING			12
#define RIGHT_BORDER_PADDING		13
#define TOP_BORDER_PADDING			14
#define BOTTOM_BORDER_PADDING		15
#define ALL_BORDER_LINE_WIDTH		16
#define LEFT_BORDER_LINE_WIDTH		17
#define RIGHT_BORDER_LINE_WIDTH		18
#define TOP_BORDER_LINE_WIDTH		19
#define BOTTOM_BORDER_LINE_WIDTH	20

// XFillBitmapItem (is also a NameOrIndex)
#define MID_BITMAP              8

// SvxHorJustifyItem
#define MID_HORJUST_HORJUST		0
#define MID_HORJUST_ADJUST		1

// SvxZoomItem
#define MID_VALUE               2

// SvxObjectItem
#define MID_START_X             1
#define MID_START_Y             2
#define MID_END_X               3
#define MID_END_Y               4
#define MID_LIMIT               5

// SvxColumnItem
#define MID_COLUMNARRAY         7
#define MID_ACTUAL              4
#define MID_TABLE               5
#define MID_ORTHO               6

// XFillGradientItem
// Don't use 0 as it used for the whole struct
#define MID_FILLGRADIENT			1
#define MID_GRADIENT_STYLE			2
#define MID_GRADIENT_STARTCOLOR		3
#define	MID_GRADIENT_ENDCOLOR		4
#define MID_GRADIENT_ANGLE			5
#define MID_GRADIENT_BORDER			6
#define MID_GRADIENT_XOFFSET		7
#define MID_GRADIENT_YOFFSET		8
#define MID_GRADIENT_STARTINTENSITY	9
#define MID_GRADIENT_ENDINTENSITY	10
#define	MID_GRADIENT_STEPCOUNT		11

// XFillHatchItem
// Don't use 0 as it used for the whole struct
#define MID_FILLHATCH				1
#define MID_HATCH_STYLE				2
#define MID_HATCH_COLOR				3
#define MID_HATCH_DISTANCE			4
#define MID_HATCH_ANGLE				5

// XLineDashItem
// Don't use 0 as it used for the whole struct
#define MID_LINEDASH				1
#define MID_LINEDASH_STYLE			2
#define MID_LINEDASH_DOTS			3
#define MID_LINEDASH_DOTLEN			4
#define MID_LINEDASH_DASHES			5
#define MID_LINEDASH_DASHLEN		6
#define MID_LINEDASH_DISTANCE		7

#define MID_LEFT                    3

#endif
