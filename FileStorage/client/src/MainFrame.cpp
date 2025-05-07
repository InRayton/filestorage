#include "MainFrame.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>

using namespace std;

enum {
    ID_LIST=wxID_HIGHEST+1,
    ID_DOWNLOAD,
    ID_UPLOAD
};

wxBEGIN_EVENT_TABLE(MainFrame,wxFrame)
    EVT_BUTTON(ID_LIST,MainFrame::OnList)//обрабатывает нажатие обновления
    EVT_BUTTON(ID_DOWNLOAD,MainFrame::OnDownload)//обрабатывает нажатие загрузки
    EVT_BUTTON(ID_UPLOAD,MainFrame::OnUpload)//обрабатывает нажатие добавления
wxEND_EVENT_TABLE()

MainFrame::MainFrame()
    :wxFrame(nullptr,wxID_ANY,"FileStorage Client",wxDefaultPosition,{600,400})
    ,client_("127.0.0.1",12345)//инициализируем клиент с адресом и портом
{
    wxPanel* p=new wxPanel(this);//создаём панель для размещения элементов
    wxBoxSizer* s=new wxBoxSizer(wxVERTICAL);//организуем вертикальную компоновку
    list_=new wxListCtrl(p,wxID_ANY,wxDefaultPosition,wxDefaultSize,
                         wxLC_REPORT|wxLC_SINGLE_SEL);//список файлов в режиме отчёта
    list_->InsertColumn(0,"Filename",wxLIST_FORMAT_LEFT,500);//добавляем колонку для имён
    s->Add(list_,1,wxEXPAND|wxALL,5);//добавляем список на всю ширину

    wxBoxSizer* btns=new wxBoxSizer(wxHORIZONTAL);//горизонтальная панель для кнопок
    btns->Add(new wxButton(p,ID_LIST,"Refresh"),0,wxALL,5);//кнопка обновить
    btns->Add(new wxButton(p,ID_DOWNLOAD,"Download"),0,wxALL,5);//кнопка скачать
    btns->Add(new wxButton(p,ID_UPLOAD,"Upload"),0,wxALL,5);//кнопка загрузить
    s->Add(btns,0,wxALIGN_CENTER);//располагаем кнопки по центру
    p->SetSizer(s);//применяем компоновку к панели
}

void MainFrame::OnList(wxCommandEvent&) {
    list_->DeleteAllItems();//очищаем текущий список
    auto files=client_.ListFiles();//получаем новые данные с сервера
    for(size_t i=0;i<files.size();++i)
        list_->InsertItem(i,files[i]);//заполняем список файлами
}

void MainFrame::OnDownload(wxCommandEvent&) {
    long idx=list_->GetNextItem(-1,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);
    if(idx==-1)return;//если ничего не выбрано, выходим
    string name=string(list_->GetItemText(idx).mb_str());//получаем имя выбранного файла
    if(client_.Download(name))
        wxMessageBox("Downloaded: "+name);//сообщаем об успешной загрузке
    else
        wxMessageBox("Failed to download");//сообщаем об ошибке
}

void MainFrame::OnUpload(wxCommandEvent&) {
    wxFileDialog dlg(this,"Select file to upload");//диалог выбора файла
    if(dlg.ShowModal()==wxID_OK) {
        string path=string(dlg.GetPath().mb_str());//получаем путь к файлу
        if(client_.Upload(path))
            wxMessageBox("Uploaded: "+path);//сообщаем об успешной загрузке
        else
            wxMessageBox("Failed to upload");//сообщаем об ошибке
    }
}
