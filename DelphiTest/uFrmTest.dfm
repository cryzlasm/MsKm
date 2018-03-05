object FrmTest: TFrmTest
  Left = 0
  Top = 0
  Caption = 'FrmTest'
  ClientHeight = 202
  ClientWidth = 447
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 32
    Top = 56
    Width = 97
    Height = 25
    Caption = #27979#35797#36755#20837#23383#20018
    TabOrder = 0
    OnClick = Button1Click
  end
  object Edit1: TEdit
    Left = 32
    Top = 24
    Width = 249
    Height = 21
    TabOrder = 1
    Text = 'ABCDabcd!@#$1234'
  end
  object Button2: TButton
    Left = 32
    Top = 88
    Width = 97
    Height = 25
    Caption = 'Button2'
    TabOrder = 2
    OnClick = Button2Click
  end
end
