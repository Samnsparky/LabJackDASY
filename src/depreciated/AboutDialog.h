#pragma once


// Import resource.h
#include "Resource.h"

class AboutDialog : public CDHtmlDialog
{
	DECLARE_DYNCREATE(AboutDialog)

public:
	AboutDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~AboutDialog();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_DIALOG1, IDH = IDR_HTML_ABOUTDIALOG };

private:
	CFont titleFont;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
