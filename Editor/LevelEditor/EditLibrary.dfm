object frmEditLibrary: TfrmEditLibrary
  Left = 747
  Top = 400
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Object Library'
  ClientHeight = 322
  ClientWidth = 308
  Color = 10528425
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Scaled = False
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 192
    Top = 0
    Width = 116
    Height = 322
    Align = alClient
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 0
    object ebCancel: TExtBtn
      Left = 4
      Top = 304
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Close'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
    object ebMakeThm: TExtBtn
      Left = 4
      Top = 140
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Make Thumbnail'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebMakeThmClick
    end
    object ebProperties: TExtBtn
      Left = 4
      Top = 121
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Properties'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebPropertiesClick
    end
    object ebSave: TExtBtn
      Left = 4
      Top = 286
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Save'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebSaveClick
    end
    object ebExportDO: TExtBtn
      Left = 4
      Top = 232
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Export DO'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebExportDOClick
    end
    object ebImport: TExtBtn
      Left = 4
      Top = 209
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Import Object'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebImportClick
    end
    object ebExportHOM: TExtBtn
      Left = 4
      Top = 250
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Export HOM'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebExportHOMClick
    end
    object ExtBtn1: TExtBtn
      Left = 4
      Top = 180
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Export Slipch'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ExtBtn1Click
    end
    object cbPreview: TCheckBox
      Left = 4
      Top = 160
      Width = 108
      Height = 17
      Caption = 'Preview'
      TabOrder = 0
      OnClick = cbPreviewClick
    end
    object paImage: TPanel
      Left = 0
      Top = 1
      Width = 116
      Height = 116
      BevelOuter = bvLowered
      Caption = '<no image>'
      ParentColor = True
      TabOrder = 1
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 114
        Height = 114
        Align = alClient
        OnPaint = pbImagePaint
      end
    end
  end
  object tvObjects: TElTree
    Left = 0
    Top = 0
    Width = 192
    Height = 322
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    Align = alLeft
    AlwaysKeepSelection = False
    AutoCollapse = False
    AutoExpand = True
    DockOrientation = doNoOrient
    DefaultSectionWidth = 120
    BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
    CustomPlusMinus = True
    DoInplaceEdit = False
    DrawFocusRect = False
    DragImageMode = dimOne
    DragTrgDrawMode = ColorRect
    ExplorerEditMode = False
    Flat = True
    FocusedSelectColor = 10526880
    FocusedSelectTextColor = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FullRowSelect = False
    GradientSteps = 64
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {
      F4FFFFFF07000000ECAFFC0000260000FFFFFFFF000001010100FC0078000000
      00000000102700000001006DC0CA5305000000000000FC00000000000000012E
      000000000000000000010000000000007445323B20757669742B2B297B0D0A20
      20202020202020202020202020202020202020742E737072696E746628227B25
      332E33662C25332E33667D2C222C757669742D3E010000000001000000000000
      000000000000ECAFFC0000260000FFFFFFFF000001010100FC00780000000000
      0000102700000001006D702B4905010000000000FC00000000000000012E0000
      00000000000000010000000000007445323B20757669742B2B297B0D0A202020
      2020202020202020202020202020202020742E737072696E746628227B25332E
      33662C25332E33667D2C222C757669742D3E0100000000010000000000000000
      00000000ECAFFC0000260000FFFFFFFF000001010100FC007800000000000000
      102700000001006D504F150E020000000000FC00000000000000012E00000000
      0000000000010000000000007445323B20757669742B2B297B0D0A2020202020
      202020202020202020202020202020742E737072696E746628227B25332E3366
      2C25332E33667D2C222C757669742D3E01000000000100000000000000000000
      0000ECAFFC0000260000FFFFFFFF000001010100FC0078000000000000001027
      00000001006D84DF650E030000000000FC00000000000000012E000000000000
      000000010000000000007445323B20757669742B2B297B0D0A20202020202020
      20202020202020202020202020742E737072696E746628227B25332E33662C25
      332E33667D2C222C757669742D3E010000000001000000000000000000000000
      ECAFFC0000260000FFFFFFFF000001010100FC00780000000000000010270000
      0001006D70981404040000000000FC00000000000000012E0000000000000000
      00010000000000007445323B20757669742B2B297B0D0A202020202020202020
      2020202020202020202020742E737072696E746628227B25332E33662C25332E
      33667D2C222C757669742D3E010000000001000000000000000000000000ECAF
      FC0000260000FFFFFFFF000001010100FC007800000000000000102700000001
      006D0427B619050000000000FC00000000000000012E00000000000000000001
      0000000000007445323B20757669742B2B297B0D0A2020202020202020202020
      202020202020202020742E737072696E746628227B25332E33662C25332E3366
      7D2C222C757669742D3E010000000001000000000000000000000000ECAFFC00
      00260000FFFFFFFF000001010100FC007800000000000000102700000001006D
      54C54504060000000000FC00000000000000012E000000000000000000010000
      000000007445323B20757669742B2B297B0D0A20202020202020202020202020
      20202020202020742E737072696E746628227B25332E33662C25332E33667D2C
      222C757669742D3E010000000001000000000000000000000000}
    HeaderFont.Charset = DEFAULT_CHARSET
    HeaderFont.Color = clWindowText
    HeaderFont.Height = -11
    HeaderFont.Name = 'MS Sans Serif'
    HeaderFont.Style = []
    HorizontalLines = True
    HorzDivLinesColor = 7368816
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.Width = 16
    HorzScrollBarStyles.ButtonSize = 14
    HorzScrollBarStyles.UseSystemMetrics = False
    HorzScrollBarStyles.UseXPThemes = False
    IgnoreEnabled = False
    IncrementalSearch = False
    ItemIndent = 14
    KeepSelectionWithinLevel = False
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    LineHeight = 16
    LinesColor = clBtnShadow
    MinusPicture.Data = {
      F6000000424DF600000000000000360000002800000008000000080000000100
      180000000000C0000000120B0000120B00000000000000000000808080808080
      808080808080808080808080808080808080808080808080808080E0E0E08080
      80808080808080808080808080808080808080E0E0E080808080808080808080
      8080808080808080E0E0E0E0E0E0E0E0E0808080808080808080808080808080
      E0E0E0E0E0E0E0E0E0808080808080808080808080E0E0E0E0E0E0E0E0E0E0E0
      E0E0E0E0808080808080808080E0E0E0E0E0E0E0E0E0E0E0E0E0E0E080808080
      8080808080808080808080808080808080808080808080808080}
    MouseFrameSelect = True
    MultiSelect = False
    OwnerDrawMask = '~~@~~'
    ParentFont = False
    PlusMinusTransparent = True
    PlusPicture.Data = {
      F6000000424DF600000000000000360000002800000008000000080000000100
      180000000000C0000000120B0000120B00000000000000000000808080808080
      8080808080808080808080808080808080808080808080808080808080808080
      80808080808080808080808080E0E0E0E0E0E080808080808080808080808080
      8080808080E0E0E0E0E0E0E0E0E0E0E0E0808080808080808080808080E0E0E0
      E0E0E0E0E0E0E0E0E0E0E0E0E0E0E0808080808080E0E0E0E0E0E0E0E0E0E0E0
      E0808080808080808080808080E0E0E0E0E0E080808080808080808080808080
      8080808080808080808080808080808080808080808080808080}
    ScrollbarOpposite = False
    ScrollTracking = True
    ShowLeafButton = False
    ShowLines = False
    SortMode = smAdd
    StoragePath = '\Tree'
    TabOrder = 1
    TabStop = True
    Tracking = False
    TrackColor = clBlack
    VertDivLinesColor = 7368816
    VerticalLines = True
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.Width = 16
    VertScrollBarStyles.ButtonSize = 14
    VertScrollBarStyles.UseSystemMetrics = False
    VertScrollBarStyles.UseXPThemes = False
    VirtualityLevel = vlNone
    UseXPThemes = False
    TextColor = clBtnText
    BkColor = clGray
    OnItemFocused = tvObjectsItemFocused
    OnDblClick = tvObjectsDblClick
    OnKeyPress = tvObjectsKeyPress
  end
  object fsStorage: TFormStorage
    StoredValues = <
      item
        Name = 'EmitterDirX'
        Value = 0s
      end
      item
        Name = 'EmitterDirY'
        Value = 0s
      end
      item
        Name = 'EmitterDirZ'
        Value = 0s
      end>
  end
end
