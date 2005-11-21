object MainForm: TMainForm
  Left = 210
  Top = 196
  BorderStyle = bsSingle
  Caption = 'MainForm'
  ClientHeight = 580
  ClientWidth = 800
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnDestroy = FormDestroy
  OnPaint = FormPaint
  OnResize = FormResize
  DesignSize = (
    800
    580)
  PixelsPerInch = 96
  TextHeight = 13
  object Panel: TPanel
    Left = 0
    Top = 32
    Width = 800
    Height = 337
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 0
    DesignSize = (
      800
      337)
    object Image: TImage
      Left = 2
      Top = 2
      Width = 796
      Height = 333
      Anchors = [akLeft, akTop, akRight, akBottom]
      OnMouseUp = ImageMouseUp
    end
  end
  object ToolBar1: TToolBar
    Left = 0
    Top = 0
    Width = 800
    Height = 29
    EdgeBorders = [ebTop, ebBottom]
    TabOrder = 1
    object NewEffectButton: TToolButton
      Left = 0
      Top = 2
      ImageIndex = 0
      OnClick = NewEffectButtonClick
    end
    object ToolButton2: TToolButton
      Left = 23
      Top = 2
      Width = 8
      Caption = 'ToolButton2'
      ImageIndex = 1
      Style = tbsSeparator
    end
    object LoadButton: TToolButton
      Left = 31
      Top = 2
      Caption = 'LoadButton'
      ImageIndex = 1
      OnClick = LoadButtonClick
    end
    object SaveButton: TToolButton
      Left = 54
      Top = 2
      Caption = 'SaveButton'
      ImageIndex = 2
      OnClick = SaveButtonClick
    end
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 561
    Width = 800
    Height = 19
    Panels = <
      item
        Text = 'Effect time : '
        Width = 50
      end>
    SimplePanel = False
  end
  object TabControl: TTabControl
    Left = 0
    Top = 372
    Width = 793
    Height = 189
    Anchors = [akLeft, akRight, akBottom]
    TabOrder = 3
    Tabs.Strings = (
      'Base color'
      'Add color'
      'Gray color'
      'Blur'
      'Gray'
      'Diality H'
      'Duality V'
      'Noise intensity'
      'Noise granularity'
      'Noise FPS')
    TabIndex = 0
    OnChange = TabControlChange
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'ppe'
    Filter = 'Postprocess files (*.ppe)|*.ppe|All files (*.*)|*.*'
    Title = 'Save postprocess effect'
    Left = 8
    Top = 36
  end
  object OpenDialog: TOpenDialog
    DefaultExt = 'ppe'
    Filter = 'Postprocess files (*.ppe)|*.ppe|All files (*.*)|*.*'
    Title = 'Open postprocess effect'
    Left = 40
    Top = 36
  end
end
