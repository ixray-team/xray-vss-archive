object frmStatistic: TfrmStatistic
  Left = 1313
  Top = 278
  Width = 544
  Height = 417
  Caption = 'Statistic'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object sgData: TElTreeStringGrid
    Left = 0
    Top = 18
    Width = 536
    Height = 353
    Cursor = crDefault
    LeftPosition = 0
    ColCount = 4
    goEditing = False
    Align = alClient
    DockOrientation = doNoOrient
    DragCursor = crDrag
    ExplorerEditMode = False
    Flat = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    GradientSteps = 64
    HeaderHeight = 17
    HeaderHotTrack = False
    HeaderSections.Data = {
      F4FFFFFF04000000585C880E00000000FFFFFFFF000001000100295C66000000
      00000000102700000001007258103F0C0000000002005072FFFFFFFF00000142
      0000000000000000000100000000000074735C4C69623B723A5C626F726C616E
      645C636F6D706F6E656E7473365C213B483A5C50726F6772616D2046696C6573
      5C5261697A655C5243335C4C696242439000000006000000436F756E74000100
      0000000000000000000000585C880E00000000FFFFFFFF000001000100295C67
      000000000000001027000000010072F8400E0C0100000002005072FFFFFFFF00
      0001420000000000000000000100000000000074735C4C69623B723A5C626F72
      6C616E645C636F6D706F6E656E7473365C213B483A5C50726F6772616D204669
      6C65735C5261697A655C5243335C4C69624243900000000A0000005075726520
      53697A650001000000000000000000000000585C880E00000000FFFFFFFF0000
      01000100295C72000000000000001027000000010072A0E2080C020000000200
      5072FFFFFFFF000001420000000000000000000100000000000074735C4C6962
      3B723A5C626F726C616E645C636F6D706F6E656E7473365C213B483A5C50726F
      6772616D2046696C65735C5261697A655C5243335C4C69624243900000000D00
      0000416C6C6F6365642053697A650001000000000000000000000000585C880E
      00000000FFFFFFFF000001000100295CD3000000000000001027000000010072
      B41EE00C0300000001005072FFFFFFFF00000142000000000000000000010000
      0000000074735C4C69623B723A5C626F726C616E645C636F6D706F6E656E7473
      365C213B483A5C50726F6772616D2046696C65735C5261697A655C5243335C4C
      69624243900000000C0000004465736372697074696F6E000100000000000000
      0000000000}
    HeaderFlat = True
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.Width = 16
    HorzScrollBarStyles.ButtonSize = 16
    Items.Data = {
      F3FFFFFF0000000080000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF83871200
      881A6B0D0000000000000000000000000000887C0000000000ED120000000000
      0000000000000000FFFFFFFF14EE120090421901D8ED1200000000001E000000
      28EE120028E12F0CECDA73068F4219018F421901246719011E2919010C421901
      B8EE000000000000FFFFFFFF00000000000000000005000000F3FFFFFF000000
      0000001200FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF8387120008061400000000
      00000000000000000000001200000000000001817C0000000000000000000000
      00FFFFFFFF0103000090000000040000005AF5024034ED120024ED1200647E02
      400400000040ED02406300000075000000ECDA730622BB8A040CED12004DBB8A
      04FFFFFFFF00000000000000000000000000F3FFFFFF0000000000001200FFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFF838712000806140000000000000000000000
      000000001200000000000001817C000000000000000000000000FFFFFFFF0103
      000090000000040000005AF5024034ED120024ED1200647E02400400000040ED
      02406300000075000000ECDA730622BB8A040CED12004DBB8A04FFFFFFFF0000
      0000000000000000000000F3FFFFFF0000000000001200FFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFF83871200080614000000000000000000000000000000120000
      0000000001817C000000000000000000000000FFFFFFFF010300009000000004
      0000005AF5024034ED120024ED1200647E02400400000040ED02406300000075
      000000ECDA730622BB8A040CED12004DBB8A04FFFFFFFF000000000000000000
      00000000F3FFFFFF0000000000001200FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      838712000806140000000000000000000000000000001200000000000001817C
      000000000000000000000000FFFFFFFF0103000090000000040000005AF50240
      34ED120024ED1200647E02400400000040ED02406300000075000000ECDA7306
      22BB8A040CED12004DBB8A04FFFFFFFF00000000000000000000000000F3FFFF
      FF0000000000001200FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF83871200080614
      0000000000000000000000000000001200000000000001817C00000000000000
      0000000000FFFFFFFF0103000090000000040000005AF5024034ED120024ED12
      00647E02400400000040ED02406300000075000000ECDA730622BB8A040CED12
      004DBB8A04FFFFFFFF00000000000000000000000000}
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    ScrollbarOpposite = False
    ShowRootButtons = False
    StoragePath = '\Tree'
    TabOrder = 0
    TabStop = True
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.Width = 16
    VertScrollBarStyles.ButtonSize = 16
    BkColor = 15790320
    OnHeaderColumnClick = sgDataHeaderColumnClick
  end
  object paCaptionMem: TPanel
    Left = 0
    Top = 0
    Width = 536
    Height = 18
    Align = alTop
    Alignment = taLeftJustify
    BevelInner = bvLowered
    Caption = ' Items'
    Color = clGray
    TabOrder = 1
  end
  object sbStatus: TStatusBar
    Left = 0
    Top = 371
    Width = 536
    Height = 19
    Panels = <
      item
        Width = 150
      end
      item
        Width = 50
      end>
    SimplePanel = False
  end
  object fsStorage: TFormStorage
    UseRegistry = True
    StoredProps.Strings = (
      'sgData.HeaderSections')
    StoredValues = <>
    Left = 8
    Top = 40
  end
end
