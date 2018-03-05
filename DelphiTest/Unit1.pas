unit Unit1;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls;

type
  TForm1 = class(TForm)
    Button1: TButton;
    Edit1: TEdit;
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

  function MsKmPressPassWord(szPassWord: PAnsiChar; dwDelay: DWORD = 100): Boolean; external 'MsKm.dll' name 'MsKmPressPassword';
var
  Form1: TForm1;

implementation

{$R *.dfm}

procedure TForm1.Button1Click(Sender: TObject);
begin
  Sleep(5000);
  MsKmPressPassWord(PAnsiChar(AnsiString(Edit1.Text)));
end;

end.
