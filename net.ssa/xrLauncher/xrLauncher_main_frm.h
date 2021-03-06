#pragma once

#include "xrLauncher_about_dlg.h"
#include "xrLauncher_benchmark_frm.h"
#include "xrLauncherControl.h"
#include "xrLauncher_utils.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#undef GetObject
namespace xrLauncher
{

	/// <summary> 
	/// Summary for xrLauncher_main_frm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class xrLauncher_main_frm : public System::Windows::Forms::Form
	{
	public: 
		xrLauncher_main_frm(void)
		{
			m_about_dlg			= 0;
			m_benchmark_dlg		= 0;
			m_settings_dlg		= 0;
			m_mod_info			= new MOD_INFO();
			InitializeComponent();
		}
	private: void Init();
	private: void InitMod();
	private: void addFileInfo(LPCSTR fn);
	public : void _Close(int res);
	public : int  _Show(int initial_state);
        
	protected: 
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private : int		m_modal_result;
	private : int		m_init_state;
	private : MOD_INFO*	m_mod_info;
	private : xrLauncher_about_dlg* m_about_dlg;
	private : xrLauncher_benchmark_frm* m_benchmark_dlg;
	private : xrLauncherControl*		m_settings_dlg;

	private: System::Windows::Forms::Panel *  panel1;
	private: System::Windows::Forms::PictureBox *  pictureBox1;
	private: System::Windows::Forms::Button *  playBtn;
	private: System::Windows::Forms::Button *  settingsBtn;
	private: System::Windows::Forms::Button *  benchmarkBtn;




	private: System::Windows::Forms::ImageList *  imageList1;
	private: System::Windows::Forms::Button *  aboutBtn;
	private: System::Windows::Forms::ImageList *  imageList2;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::ComboBox *  modListCombo;
	private: System::Windows::Forms::Button *  button1;

	private: System::ComponentModel::IContainer *  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = new System::ComponentModel::Container();
			System::Resources::ResourceManager *  resources = new System::Resources::ResourceManager(__typeof(xrLauncher::xrLauncher_main_frm));
			this->panel1 = new System::Windows::Forms::Panel();
			this->modListCombo = new System::Windows::Forms::ComboBox();
			this->label1 = new System::Windows::Forms::Label();
			this->aboutBtn = new System::Windows::Forms::Button();
			this->imageList1 = new System::Windows::Forms::ImageList(this->components);
			this->benchmarkBtn = new System::Windows::Forms::Button();
			this->settingsBtn = new System::Windows::Forms::Button();
			this->playBtn = new System::Windows::Forms::Button();
			this->pictureBox1 = new System::Windows::Forms::PictureBox();
			this->imageList2 = new System::Windows::Forms::ImageList(this->components);
			this->button1 = new System::Windows::Forms::Button();
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->panel1->BackgroundImage = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"panel1.BackgroundImage")));
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel1->Controls->Add(this->button1);
			this->panel1->Controls->Add(this->modListCombo);
			this->panel1->Controls->Add(this->label1);
			this->panel1->Controls->Add(this->aboutBtn);
			this->panel1->Controls->Add(this->benchmarkBtn);
			this->panel1->Controls->Add(this->settingsBtn);
			this->panel1->Controls->Add(this->playBtn);
			this->panel1->Controls->Add(this->pictureBox1);
			this->panel1->Location = System::Drawing::Point(0, 0);
			this->panel1->Name = S"panel1";
			this->panel1->Size = System::Drawing::Size(512, 256);
			this->panel1->TabIndex = 0;
			// 
			// modListCombo
			// 
			this->modListCombo->BackColor = System::Drawing::Color::Silver;
			this->modListCombo->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->modListCombo->Location = System::Drawing::Point(144, 224);
			this->modListCombo->Name = S"modListCombo";
			this->modListCombo->Size = System::Drawing::Size(256, 21);
			this->modListCombo->TabIndex = 16;
			this->modListCombo->SelectedIndexChanged += new System::EventHandler(this, modList_SelectedIndexChanged);
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Location = System::Drawing::Point(144, 8);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(256, 136);
			this->label1->TabIndex = 15;
			this->label1->Text = S"Chernobyl zone was lit by an intolerably bright light. The clouds were evaporatin" 
				S"g in the silver bright sky with a thunder and earthquake to follow. People fell " 
				S"on the ground facedown closing their bleeding eyes and ears. The glow spread ove" 
				S"r an immense territory which was subsequently called the Zone. People ran away s" 
				S"aving their lives. It looked like a radioactive explosion at the nuclear plant o" 
				S"ccurred.";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// aboutBtn
			// 
			this->aboutBtn->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->aboutBtn->BackColor = System::Drawing::Color::Transparent;
			this->aboutBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->aboutBtn->ForeColor = System::Drawing::SystemColors::ControlText;
			this->aboutBtn->ImageList = this->imageList1;
			this->aboutBtn->Location = System::Drawing::Point(416, 144);
			this->aboutBtn->Name = S"aboutBtn";
			this->aboutBtn->Size = System::Drawing::Size(86, 26);
			this->aboutBtn->TabIndex = 14;
			this->aboutBtn->Text = S"About";
			this->aboutBtn->Click += new System::EventHandler(this, aboutBtn_Click);
			// 
			// imageList1
			// 
			this->imageList1->ImageSize = System::Drawing::Size(86, 26);
			this->imageList1->ImageStream = (__try_cast<System::Windows::Forms::ImageListStreamer *  >(resources->GetObject(S"imageList1.ImageStream")));
			this->imageList1->TransparentColor = System::Drawing::SystemColors::Control;
			// 
			// benchmarkBtn
			// 
			this->benchmarkBtn->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->benchmarkBtn->BackColor = System::Drawing::Color::Transparent;
			this->benchmarkBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->benchmarkBtn->ImageList = this->imageList1;
			this->benchmarkBtn->Location = System::Drawing::Point(416, 104);
			this->benchmarkBtn->Name = S"benchmarkBtn";
			this->benchmarkBtn->Size = System::Drawing::Size(86, 26);
			this->benchmarkBtn->TabIndex = 2;
			this->benchmarkBtn->Text = S"Benchmark";
			this->benchmarkBtn->Click += new System::EventHandler(this, benchmarkBtn_Click);
			// 
			// settingsBtn
			// 
			this->settingsBtn->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->settingsBtn->BackColor = System::Drawing::Color::Transparent;
			this->settingsBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->settingsBtn->ForeColor = System::Drawing::SystemColors::ControlText;
			this->settingsBtn->Location = System::Drawing::Point(416, 64);
			this->settingsBtn->Name = S"settingsBtn";
			this->settingsBtn->Size = System::Drawing::Size(86, 26);
			this->settingsBtn->TabIndex = 1;
			this->settingsBtn->Text = S"Settings";
			this->settingsBtn->Click += new System::EventHandler(this, settingsBtn_Click);
			// 
			// playBtn
			// 
			this->playBtn->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->playBtn->BackColor = System::Drawing::Color::Transparent;
			this->playBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->playBtn->ImageList = this->imageList1;
			this->playBtn->Location = System::Drawing::Point(416, 24);
			this->playBtn->Name = S"playBtn";
			this->playBtn->Size = System::Drawing::Size(86, 26);
			this->playBtn->TabIndex = 0;
			this->playBtn->Text = S"Play";
			this->playBtn->Click += new System::EventHandler(this, playBtn_Click);
			this->playBtn->EnabledChanged += new System::EventHandler(this, btn_enabled_changed);
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::Color::Transparent;
			this->pictureBox1->BackgroundImage = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"pictureBox1.BackgroundImage")));
			this->pictureBox1->Location = System::Drawing::Point(8, 8);
			this->pictureBox1->Name = S"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(120, 240);
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			// 
			// imageList2
			// 
			this->imageList2->ImageSize = System::Drawing::Size(128, 256);
			this->imageList2->ImageStream = (__try_cast<System::Windows::Forms::ImageListStreamer *  >(resources->GetObject(S"imageList2.ImageStream")));
			this->imageList2->TransparentColor = System::Drawing::Color::Transparent;
			// 
			// button1
			// 
			this->button1->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button1->BackColor = System::Drawing::Color::Transparent;
			this->button1->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button1->ImageList = this->imageList1;
			this->button1->Location = System::Drawing::Point(416, 224);
			this->button1->Name = S"button1";
			this->button1->Size = System::Drawing::Size(86, 26);
			this->button1->TabIndex = 17;
			this->button1->Text = S"Quit";
			this->button1->Click += new System::EventHandler(this, button1_Click);
			// 
			// xrLauncher_main_frm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(512, 256);
			this->Controls->Add(this->panel1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = S"xrLauncher_main_frm";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = S"xrLauncher_main_frm";
			this->KeyDown += new System::Windows::Forms::KeyEventHandler(this, xrLauncher_main_frm_KeyDown);
			this->panel1->ResumeLayout(false);
			this->ResumeLayout(false);

		}		



private: System::Void modList_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e);

private: System::Void playBtn_Click(System::Object *  sender, System::EventArgs *  e);




private: System::Void btn_mouse_down(System::Object *  sender, System::Windows::Forms::MouseEventArgs *  e)
		 {
/*			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(!btn->Enabled)
				return;

			btn->ImageIndex = 1;*/
		 }

private: System::Void btn_mouse_up(System::Object *  sender, System::Windows::Forms::MouseEventArgs *  e)
		 {
/*			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(!btn->Enabled)
				return;

			btn->ImageIndex = 0;*/
		 }



private: System::Void btn_enabled_changed(System::Object *  sender, System::EventArgs *  e)
		 {
/*			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(btn->Enabled)
				btn->ImageIndex = 0;
			btn->ImageIndex = 2;*/
		
		 }

private: System::Void btn_key_down(System::Object *  sender, System::Windows::Forms::KeyEventArgs *  e)
		 {
/*			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(!(e->KeyCode == Keys::Space))
				return;

			if(!btn->Enabled)
				return;

			btn->ImageIndex = 1;
*/
		 }

private: System::Void btn_key_up(System::Object *  sender, System::Windows::Forms::KeyEventArgs *  e)
		 {
/*			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(!(e->KeyCode == Keys::Space))
				return;

			if(!btn->Enabled)
				return;

			btn->ImageIndex = 0;
*/
		 }

private: System::Void aboutBtn_Click(System::Object *  sender, System::EventArgs *  e);

private: System::Void benchmarkBtn_Click(System::Object *  sender, System::EventArgs *  e);

private: System::Void xrLauncher_main_frm_KeyDown(System::Object *  sender, System::Windows::Forms::KeyEventArgs *  e)
		 {
/*			 if(e->Alt&&e->KeyCode == System::Windows::Forms::Keys::F4)
				_Close(0);*/
		 }

private: System::Void settingsBtn_Click(System::Object *  sender, System::EventArgs *  e);

private: System::Void button1_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 _Close(8);
		 }

};
}