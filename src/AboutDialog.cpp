// AboutDialog.cpp : implementation file 
//

#include "stdafx.h"
#include "LabJackDasy.h"
#include "AboutDialog.h"


// AboutDialog dialog

IMPLEMENT_DYNCREATE(AboutDialog, CDHtmlDialog)

AboutDialog::AboutDialog(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(AboutDialog::IDD, AboutDialog::IDH, pParent)
{
}

AboutDialog::~AboutDialog()
{
	titleFont.DeleteObject();
}

void AboutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL AboutDialog::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

	// Set title label font
	titleFont.CreatePointFont(180, _T("Arial"));
	GetDlgItem(IDC_TITLE)->SetFont(&titleFont);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(AboutDialog, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(AboutDialog)
DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
END_DHTML_EVENT_MAP()



// AboutDialog message handlers

HRESULT AboutDialog::OnButtonOK(IHTMLElement* /*pElement*/)
{
	CloseWindow();
	return S_OK;  // return TRUE  unless you set the focus to a control
}
