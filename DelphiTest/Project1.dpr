program Project1;

uses
  Vcl.Forms,
  uFrmTest in 'uFrmTest.pas' {FrmTest};

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TFrmTest, FrmTest);
  Application.Run;
end.
