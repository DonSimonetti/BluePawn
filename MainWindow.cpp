//
// Created by matteo on 16/09/18.
//

#include <sstream>
#include "MainWindow.h"
#include "CompilerDialog.h"
#include "IncludeDirDialog.h"
#include "WindowsIDs.h"

using namespace std;

MainWindow::MainWindow() : wxFrame(nullptr,wxID_ANY,"BluePawn",wxDefaultPosition,wxSize(1024,768)), includesPath("/include"), currentFile(new PawnDocument("")), compileFlags("-i'"+includesPath+"' ")
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    CreateMenuBar();

    CreateToolbar();

    CreateSplittedWindow();

    StatusBar();

    this->Layout();

    this->Centre( wxBOTH );

    SetupBindings();

    ResetAppName();
}

void MainWindow::SetupBindings()
{
    Bind(wxEVT_TOOL,&MainWindow::OnNewPage,this,ID_New);
    Bind(wxEVT_TOOL,&MainWindow::OnOpenFile,this,ID_Open);
    Bind(wxEVT_TOOL,&MainWindow::OnSaveFile,this,ID_Save);
    Bind(wxEVT_TOOL,&MainWindow::OnSaveFileAs,this,ID_SaveAs);
    Bind(wxEVT_TOOL,&MainWindow::OnCompile,this,ID_Compile);
    Bind(wxEVT_TOOL,&MainWindow::OnChangeIncludesFolder,this,ID_IncFolder);
    Bind(wxEVT_STC_MODIFIED,&MainWindow::OnTypeText,this,ID_TextEditor);
}

void MainWindow::CreateMenuBar()
{
    menuBar = new wxMenuBar( 0 );

    fileMenu = new wxMenu();

    wxMenuItem* newMenuItem=new wxMenuItem( fileMenu, ID_New, "New");
    fileMenu->Append( newMenuItem );

    wxMenuItem * openMenuItem=new wxMenuItem(fileMenu,ID_Open,"Open");
    fileMenu->Append(openMenuItem);

    wxMenuItem * saveMenuItem=new wxMenuItem(fileMenu,ID_Save,"Save");
    fileMenu->Append(saveMenuItem);

    wxMenuItem * saveasMenuItem=new wxMenuItem(fileMenu,ID_SaveAs,"Save as");
    fileMenu->Append(saveasMenuItem);

    menuBar->Append( fileMenu, "File");

    buildMenu=new wxMenu();
    wxMenuItem * buildMenuItem=new wxMenuItem(buildMenu,ID_Compile,"Build");
    buildMenu->Append(buildMenuItem);
    wxMenuItem * lastBuildMenuItem=new wxMenuItem(buildMenu,ID_LastBuild,"Last build");
    buildMenu->Append(lastBuildMenuItem);

    menuBar->Append(buildMenu,"Build");

    settingsMenu=new wxMenu();

    wxMenuItem* includesMenuItem=new wxMenuItem( fileMenu, ID_IncFolder, "Includes folder");
    settingsMenu->Append( includesMenuItem );

    /*wxMenuItem* compilerMenuItem=new wxMenuItem( fileMenu, wxID_ANY, "Compiler");
    settingsMenu->Append( compilerMenuItem );*/

    menuBar->Append(settingsMenu,"Settings");

    this->SetMenuBar( menuBar );
}

void MainWindow::CreateToolbar()
{
    toolBar = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY );
    newDocTool = toolBar->AddTool( ID_New, wxT("tool"), wxArtProvider::GetBitmap( wxART_NORMAL_FILE, wxART_BUTTON ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

    openTool = toolBar->AddTool( ID_Open, wxT("tool"), wxArtProvider::GetBitmap( wxART_FOLDER_OPEN, wxART_BUTTON ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

    saveTool = toolBar->AddTool( ID_Save, wxT("tool"), wxArtProvider::GetBitmap( wxART_FLOPPY, wxART_BUTTON ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

    m_tool4 = toolBar->AddTool( wxID_ANY, wxT("tool"), wxArtProvider::GetBitmap( wxT("gtk-find-and-replace"), wxART_BUTTON ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

    compileTool = toolBar->AddTool( ID_Compile, wxT("tool"), wxArtProvider::GetBitmap( wxT("gtk-execute"), wxART_BUTTON ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

    m_tool7 = toolBar->AddTool( wxID_ANY, wxT("tool"), wxArtProvider::GetBitmap( wxART_GO_FORWARD, wxART_BUTTON ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

    toolBar->Realize();
}

void MainWindow::CreateSplittedWindow()
{
    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer( wxVERTICAL );

    m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
    //m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( MainWindow::m_splitter1OnIdle ), NULL, this );

    panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer2;
    bSizer2 = new wxBoxSizer( wxVERTICAL );

    CreateTextEditor();
    bSizer2->Add( textEditor, 1, wxEXPAND | wxALL, 5 );


    panel1->SetSizer( bSizer2 );
    panel1->Layout();
    bSizer2->Fit( panel1 );
    m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer3;
    bSizer3 = new wxBoxSizer( wxVERTICAL );

    m_treeListCtrl1 = new wxTreeListCtrl( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_DEFAULT_STYLE );

    bSizer3->Add( m_treeListCtrl1, 1, wxEXPAND | wxALL, 5 );

    m_treeListCtrl1->AppendColumn("Includes");
    wxTreeListItem asamp=m_treeListCtrl1->AppendItem(m_treeListCtrl1->GetRootItem(),"a_samp.inc");
    m_treeListCtrl1->AppendItem(asamp,"funzione()");


    m_panel2->SetSizer( bSizer3 );
    m_panel2->Layout();
    bSizer3->Fit( m_panel2 );
    m_splitter1->SplitVertically( panel1, m_panel2, 304 );
    bSizer1->Add( m_splitter1, 1, wxEXPAND, 5 );
    wxSize i=wxFrame::GetSize();
    m_splitter1->SetSashPosition(static_cast<int>(i.x * 0.8));

    this->SetSizer( bSizer1 );
}

void MainWindow::CreateTextEditor()
{
    textEditor = new wxStyledTextCtrl( panel1, ID_TextEditor, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString );
    /*
    textEditor->SetBackSpaceUnIndents( true );
    textEditor->SetViewEOL( false );
    textEditor->SetViewWhiteSpace( false );
    textEditor->SetMarginWidth( 2, 0 );
    textEditor->SetIndentationGuides( true );
    textEditor->SetMarginType( 1, wxSTC_MARGIN_SYMBOL );
    textEditor->SetMarginMask( 1, wxSTC_MASK_FOLDERS );
    textEditor->SetMarginWidth( 1, 16);
    textEditor->SetMarginSensitive( 1, true );
    textEditor->SetProperty( wxT("fold"), wxT("1") );
    textEditor->SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
    textEditor->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
    textEditor->SetMarginWidth( 0, textEditor->TextWidth( wxSTC_STYLE_LINENUMBER, wxT("_99999") ) );
    textEditor->MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS );
    textEditor->MarkerSetBackground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
    textEditor->MarkerSetForeground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
    textEditor->MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS );
    textEditor->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
    textEditor->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
    textEditor->MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY );
    textEditor->MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS );
    textEditor->MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
    textEditor->MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
    textEditor->MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS );
    textEditor->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
    textEditor->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
    textEditor->MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY );
    textEditor->MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY );
    textEditor->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
    textEditor->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );*/

    /*textEditor->SetUseTabs( true );
    textEditor->SetTabWidth( 4 );
    textEditor->SetIndent( 4 );*/
    textEditor->SetTabIndents( true );

    textEditor->StyleSetSize(wxSTC_STYLE_DEFAULT,11);

    textEditor->SetLexer(wxSTC_LEX_CPP);

    textEditor->StyleSetForeground (wxSTC_C_STRING,            wxColour(150,0,0));
    textEditor->StyleSetForeground (wxSTC_C_PREPROCESSOR,      wxColour(165,105,0));
    textEditor->StyleSetForeground (wxSTC_C_IDENTIFIER,        wxColour(40,0,60));
    textEditor->StyleSetForeground (wxSTC_C_NUMBER,            wxColour(0,150,0));
    textEditor->StyleSetForeground (wxSTC_C_CHARACTER,         wxColour(150,0,0));
    textEditor->StyleSetForeground (wxSTC_C_WORD,              wxColour(0,0,150));
    textEditor->StyleSetForeground (wxSTC_C_WORD2,             wxColour(0,150,0));
    textEditor->StyleSetForeground (wxSTC_C_COMMENT,           wxColour(150,150,150));
    textEditor->StyleSetForeground (wxSTC_C_COMMENTLINE,       wxColour(150,150,150));
    textEditor->StyleSetForeground (wxSTC_C_COMMENTDOC,        wxColour(150,150,150));
    textEditor->StyleSetForeground (wxSTC_C_COMMENTDOCKEYWORD, wxColour(0,0,200));
    textEditor->StyleSetForeground (wxSTC_C_COMMENTDOCKEYWORDERROR, wxColour(0,0,200));
    textEditor->StyleSetBold(wxSTC_C_WORD, true);
    textEditor->StyleSetBold(wxSTC_C_WORD2, true);
    textEditor->StyleSetBold(wxSTC_C_COMMENTDOCKEYWORD, true);

    // a sample list of keywords, I haven't included them all to keep it short...
    textEditor->SetKeyWords(0, wxT("return for while break continue do if else false true enum case default Float goto sizeof stock public new native forward switch"));
    //textEditor->SetKeyWords(1, wxT("const"));
}

void MainWindow::StatusBar()
{
    statusBar = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );

    statusBar->SetStatusText("BluePawn by DonSimonetti");
}

void MainWindow::OnNewPage(wxCommandEvent &event)
{
    if(currentFile->isEdited)
    {
        wxMessageDialog newPage(this,"Warning: Any unsaved edit will be lost. Continue?","New Script",wxYES_NO|wxCENTRE);

        if(newPage.ShowModal()==wxID_YES)
        {
            textEditor->SetText("");
            delete currentFile;
            currentFile=new PawnDocument("");
        }
    }
    else
    {
        textEditor->SetText("");
        delete currentFile;
        currentFile=new PawnDocument("");
    }
    ResetAppName();
}

void MainWindow::OnOpenFile(wxCommandEvent &event)
{
    wxFileDialog fileDialog(this,"Open PWN fileMenu","","","Pawn files (*.pwn)|*.pwn|Pawn include files (*.inc)|*.inc|Other files ...|*",wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    if(currentFile && currentFile->isEdited)
    {
        wxMessageDialog loadMessage(this, "document not saved", "MSG", wxICON_WARNING | wxCENTRE | wxOK);

        if(loadMessage.ShowModal()==wxID_OK)
        {
            ShowFileDialog(fileDialog);
        }
    }
    else
        ShowFileDialog(fileDialog);
}

void MainWindow::ShowFileDialog(wxFileDialog & fileDialog)
{
    if(fileDialog.ShowModal()==wxID_OK)
    {
        string path=fileDialog.GetPath().ToStdString();
        if(path.length())
        {
            if(currentFile)
                delete currentFile;

            currentFile=new PawnDocument(path);
            currentFile->WriteInTextEditor(*textEditor);
            currentFile->isEdited=false;

            ResetAppName();
        }
        else
            wxMessageBox("Cannot open this file","Error",wxICON_ERROR);
    }
}

void MainWindow::OnSaveFile(wxCommandEvent &event)
{
    if(!currentFile->GetPath().length())
        OnSaveFileAs(event);
    else
        currentFile->Save(textEditor->GetText().ToStdString());

    ResetAppName();
}

void MainWindow::OnSaveFileAs(wxCommandEvent &event)
{
    wxFileDialog saveDialog(this,"Save","","","Pawn files (*.pwn)|*.pwn|Pawn include files (*.inc)|*.inc",wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    if(saveDialog.ShowModal()==wxID_OK)
    {
        string path=saveDialog.GetPath().ToStdString();

        //Check for file extension
        if(path.substr(path.find_last_of("/")+1,path.length()).find(".")==string::npos)
        {
            path.append(".pwn");
        }

        currentFile->SetPath(path);
        currentFile->Save(textEditor->GetText().ToStdString());

        ResetAppName();
    }
}

void MainWindow::OnCompile(wxCommandEvent &event)
{
    if(includesPath.length())
    {
        if (currentFile->GetPath().length())
        {
            currentFile->Save(textEditor->GetText().ToStdString());
            string output;
            FILE *stream;
            const int max_buff = 512;
            char buffer[max_buff];

            string cmd = "./pawncc '" + currentFile->GetPath() + "'" +" -D'"+currentFile->GetPath().substr(0,currentFile->GetPath().find_last_of("/"))+"' " + compileFlags;
            cmd.append(" 2>&1");

            CompilerDialog *compilerDialog = new CompilerDialog(this);

            stream = popen(cmd.c_str(), "r");
            if (stream)
            {
                while (!feof(stream))
                {
                    if (fgets(buffer, max_buff, stream) != nullptr)
                        output.append(buffer);
                }
                pclose(stream);

                compilerDialog->Write(output);
            }
            else
                wxMessageBox("Unknown command", "Error", wxICON_ERROR);

            ResetAppName();
        }
        else
            wxMessageBox("PWN file should be saved first", "Error", wxICON_ERROR);
    }
    else
        wxMessageBox("No includes folder selected","Error",wxICON_ERROR);
}

void MainWindow::OnChangeIncludesFolder(wxCommandEvent &event)
{
    IncludeDirDialog * includeDialog=new IncludeDirDialog(this,wxID_ANY);

    includeDialog->Show();

    cout << includeDialog->GetPath()<<endl;
}

void MainWindow::OnTypeText(wxStyledTextEvent &event)
{
    if(currentFile)
    {
        currentFile->isEdited=true;
        ResetAppName();
    }
}

void MainWindow::SaveFile(const string & path)
{
    ofstream outputFile;
    outputFile.open(path);
    outputFile << path<<endl;
    outputFile.close();
}

void MainWindow::ResetAppName()
{
    if(currentFile)
    {
        string title="BluePawn";
        title.append(" [");

        if(currentFile->isEdited)
            title.append("*");

        title.append(currentFile->GetName()+"]");

        SetTitle(title);
    }
}