unit uFrmTest;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls;

type
  TFrmTest = class(TForm)
    Button1: TButton;
    Edit1: TEdit;
    Button2: TButton;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  FrmTest: TFrmTest;

  function MsKmPressPassWord(szPassWord: PAnsiChar; dwDelay: DWORD): Boolean; stdcall; external 'MsKm.dll' name 'MsKmPressPassword';

implementation

{$R *.dfm}

procedure TFrmTest.Button1Click(Sender: TObject);
var
  ps: PAnsiChar;
begin
  Sleep(3000);
  ps := PAnsiChar(AnsiString(Edit1.Text));
  OutputDebugStringA(ps);
  MsKmPressPassWord(ps, 100);
  OutputDebugString('MS - 输入完成...');
end;

procedure TFrmTest.Button2Click(Sender: TObject);
var
  h: HWND;
begin
  h := CreateFileA('\\.\kmclass',
                   GENERIC_READ,
                   0,
                   nil,
                   OPEN_EXISTING,
                   FILE_ATTRIBUTE_NORMAL,
                   0);
  if h = INVALID_HANDLE_VALUE then
  begin
    ShowMessage(Format('创建文件失败 %d', [GetLastError]));
  end;
  CloseHandle(h);
end;

end.
