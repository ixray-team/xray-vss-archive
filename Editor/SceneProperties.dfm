object frmSceneProperties: TfrmSceneProperties
  Left = 337
  Top = 448
  BorderStyle = bsDialog
  Caption = 'Build options'
  ClientHeight = 300
  ClientWidth = 496
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pcBuildOptions: TPageControl
    Left = 134
    Top = 0
    Width = 362
    Height = 279
    ActivePage = tsLight
    Align = alClient
    HotTrack = True
    TabIndex = 4
    TabOrder = 1
    object tsBuildOptions: TTabSheet
      Caption = 'Build Options'
      ImageIndex = 8
      object RxLabel25: TLabel
        Left = 107
        Top = 106
        Width = 126
        Height = 16
        Caption = 'Editor Build Options...'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ParentFont = False
      end
    end
    object tsLevelOptions: TTabSheet
      Caption = 'Level options'
      object Panel2: TPanel
        Left = 0
        Top = 0
        Width = 354
        Height = 73
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
        object RxLabel2: TLabel
          Left = 3
          Top = 12
          Width = 50
          Height = 13
          Caption = 'Level path'
        end
        object Label2: TLabel
          Left = 3
          Top = 31
          Width = 54
          Height = 13
          Caption = 'Level prefix'
        end
        object edLevelPath: TEdit
          Left = 98
          Top = 9
          Width = 254
          Height = 18
          AutoSize = False
          CharCase = ecLowerCase
          TabOrder = 0
        end
        object edLevelPrefix: TEdit
          Left = 98
          Top = 28
          Width = 254
          Height = 18
          AutoSize = False
          CharCase = ecLowerCase
          TabOrder = 1
        end
      end
    end
    object tsLevelScript: TTabSheet
      Caption = 'Level script'
      ImageIndex = 9
      object mmText: TRichEdit
        Left = 0
        Top = 0
        Width = 354
        Height = 251
        Align = alClient
        ScrollBars = ssVertical
        TabOrder = 0
      end
    end
    object tsOptimizing: TTabSheet
      Caption = 'Optimizing'
      ImageIndex = 1
      object RxLabel11: TLabel
        Left = 2
        Top = 10
        Width = 99
        Height = 13
        Caption = 'Normal smooth angle'
      end
      object RxLabel20: TLabel
        Left = 3
        Top = 30
        Width = 85
        Height = 13
        Caption = 'Weld distance (m)'
      end
      object seSMAngle: TMultiObjSpinEdit
        Left = 115
        Top = 8
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 1000
        ValueType = vtFloat
        Value = 89
        AutoSize = False
        TabOrder = 0
      end
      object seWeldDistance: TMultiObjSpinEdit
        Left = 115
        Top = 28
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 3
        Increment = 0.001
        MaxValue = 10
        ValueType = vtFloat
        Value = 0.001
        AutoSize = False
        TabOrder = 1
      end
    end
    object tsLight: TTabSheet
      Caption = 'Light'
      ImageIndex = 4
      object RxLabel5: TLabel
        Left = 2
        Top = 10
        Width = 74
        Height = 13
        Caption = 'Pixels per meter'
      end
      object RxLabel32: TLabel
        Left = 2
        Top = 29
        Width = 111
        Height = 13
        Caption = 'Error (for LM collapsing)'
      end
      object MxLabel1: TLabel
        Left = 2
        Top = 49
        Width = 84
        Height = 13
        Caption = 'Error (for LM zero)'
        WordWrap = True
      end
      object Label1: TLabel
        Left = 2
        Top = 75
        Width = 63
        Height = 13
        Caption = 'Jitter samples'
        WordWrap = True
      end
      object seLMPixelsPerMeter: TMultiObjSpinEdit
        Left = 114
        Top = 8
        Width = 95
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 20
        ValueType = vtFloat
        Value = 5
        AutoSize = False
        TabOrder = 3
      end
      object seLMRMS: TMultiObjSpinEdit
        Left = 114
        Top = 28
        Width = 95
        Height = 18
        LWSensitivity = 0.01
        ButtonKind = bkLightWave
        MaxValue = 255
        Value = 8
        AutoSize = False
        TabOrder = 1
      end
      object seLMRMSZero: TMultiObjSpinEdit
        Left = 114
        Top = 48
        Width = 95
        Height = 18
        LWSensitivity = 0.01
        ButtonKind = bkLightWave
        MaxValue = 255
        Value = 8
        AutoSize = False
        TabOrder = 2
      end
      object rgLMJitterSamples: TRadioGroup
        Left = 114
        Top = 66
        Width = 95
        Height = 30
        Columns = 3
        ItemIndex = 1
        Items.Strings = (
          '1'
          '4'
          '9')
        TabOrder = 0
      end
    end
    object tsProgressive: TTabSheet
      Caption = 'Progressive'
      ImageIndex = 5
      object RxLabel15: TLabel
        Left = 2
        Top = 28
        Width = 49
        Height = 13
        Caption = 'UV weight'
      end
      object RxLabel16: TLabel
        Left = 2
        Top = 47
        Width = 71
        Height = 13
        Caption = 'Position weight'
      end
      object RxLabel17: TLabel
        Left = 2
        Top = 67
        Width = 80
        Height = 13
        Caption = 'Curvature weight'
      end
      object RxLabel26: TLabel
        Left = 2
        Top = 88
        Width = 93
        Height = 13
        Caption = 'Higher border angle'
      end
      object RxLabel27: TLabel
        Left = 2
        Top = 107
        Width = 107
        Height = 13
        Caption = 'Higher border distance'
      end
      object RxLabel28: TLabel
        Left = 2
        Top = 127
        Width = 175
        Height = 16
        AutoSize = False
        Caption = 'Heuristic (Stripify vs Progressive)'
        WordWrap = True
      end
      object sePMUV: TMultiObjSpinEdit
        Left = 162
        Top = 26
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 1000
        ValueType = vtFloat
        Value = 0.5
        AutoSize = False
        TabOrder = 0
      end
      object sePMPos: TMultiObjSpinEdit
        Left = 162
        Top = 46
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 1000
        ValueType = vtFloat
        Value = 1
        AutoSize = False
        TabOrder = 1
      end
      object sePMCurv: TMultiObjSpinEdit
        Left = 162
        Top = 66
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 1000
        ValueType = vtFloat
        Value = 1
        AutoSize = False
        TabOrder = 2
      end
      object cbProgressive: TCheckBox
        Left = 1
        Top = 8
        Width = 174
        Height = 17
        Alignment = taLeftJustify
        Caption = 'Convert to progressive'
        TabOrder = 3
      end
      object sePM_borderH_angle: TMultiObjSpinEdit
        Left = 162
        Top = 86
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 360
        ValueType = vtFloat
        Value = 150
        AutoSize = False
        TabOrder = 4
      end
      object sePM_borderH_distance: TMultiObjSpinEdit
        Left = 162
        Top = 106
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 100
        ValueType = vtFloat
        Value = 0.05
        AutoSize = False
        TabOrder = 5
      end
      object sePM_heuristic: TMultiObjSpinEdit
        Left = 162
        Top = 126
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 1
        ValueType = vtFloat
        Value = 0.85
        AutoSize = False
        TabOrder = 6
      end
    end
  end
  object tvOptions: TElTree
    Left = 0
    Top = 0
    Width = 134
    Height = 279
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    Align = alLeft
    AlwaysKeepFocus = True
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
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FullRowSelect = False
    GradientSteps = 64
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {
      F4FFFFFF070000002410D71200000000FFFFFFFF0000010101005F5278000000
      00000000102700000001004520E1C6050000000000005F4C0000000000000149
      00000000000000000001000000000000000000000800000076636C782E6C6962
      000019018410D7128410D71230000000656C7061636B42362E6C696220656469
      746F722E6C696220626362736D702E6C90000000010000000001000000000000
      0000000000002410D71200000000FFFFFFFF0000010101005F52780000000000
      00001027000000010045E8E5C6050100000000005F4C00000000000001490000
      0000000000000001000000000000000000000800000076636C782E6C69620000
      19018410D7128410D71230000000656C7061636B42362E6C696220656469746F
      722E6C696220626362736D702E6C900000000100000000010000000000000000
      000000002410D71200000000FFFFFFFF0000010101005F527800000000000000
      1027000000010045ACE6C6050200000000005F4C000000000000014900000000
      000000000001000000000000000000000800000076636C782E6C696200001901
      8410D7128410D71230000000656C7061636B42362E6C696220656469746F722E
      6C696220626362736D702E6C9000000001000000000100000000000000000000
      00002410D71200000000FFFFFFFF0000010101005F5278000000000000001027
      00000001004570E7C6050300000000005F4C0000000000000149000000000000
      00000001000000000000000000000800000076636C782E6C6962000019018410
      D7128410D71230000000656C7061636B42362E6C696220656469746F722E6C69
      6220626362736D702E6C90000000010000000001000000000000000000000000
      2410D71200000000FFFFFFFF0000010101005F52780000000000000010270000
      0001004534E8C6050400000000005F4C00000000000001490000000000000000
      0001000000000000000000000800000076636C782E6C6962000019018410D712
      8410D71230000000656C7061636B42362E6C696220656469746F722E6C696220
      626362736D702E6C900000000100000000010000000000000000000000002410
      D71200000000FFFFFFFF0000010101005F527800000000000000102700000001
      0045F8E8C6050500000000005F4C000000000000014900000000000000000001
      000000000000000000000800000076636C782E6C6962000019018410D7128410
      D71230000000656C7061636B42362E6C696220656469746F722E6C6962206263
      62736D702E6C900000000100000000010000000000000000000000002410D712
      00000000FFFFFFFF0000010101005F5278000000000000001027000000010045
      BCE9C6050600000000005F4C0000000000000149000000000000000000010000
      00000000000000000800000076636C782E6C6962000019018410D7128410D712
      30000000656C7061636B42362E6C696220656469746F722E6C69622062636273
      6D702E6C90000000010000000001000000000000000000000000}
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
    LinesColor = 5329233
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
    ShowCheckboxes = True
    ShowLeafButton = False
    ShowLines = False
    StoragePath = '\Tree'
    TabOrder = 0
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
    OnItemSelectedChange = tvOptionsItemSelectedChange
  end
  object Panel1: TPanel
    Left = 0
    Top = 279
    Width = 496
    Height = 21
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 2
    object ebOk: TExtBtn
      Left = 335
      Top = 1
      Width = 80
      Height = 20
      Align = alNone
      BevelShow = False
      Caption = 'Ok'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = btContinueClick
    end
    object ebCancel: TExtBtn
      Left = 415
      Top = 1
      Width = 80
      Height = 20
      Align = alNone
      BevelShow = False
      Caption = 'Cancel'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
  end
  object fsSceneProps: TFormStorage
    IniSection = 'Build Options'
    Options = [fpPosition]
    RegistryRoot = prLocalMachine
    StoredProps.Strings = (
      'pcBuildOptions.ActivePage')
    StoredValues = <>
    Left = 4
    Top = 2
  end
end
