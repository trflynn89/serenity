#include <LibGUI/GWindow.h>
#include <LibGUI/GWidget.h>
#include <LibGUI/GBoxLayout.h>
#include <LibGUI/GApplication.h>
#include <LibGUI/GStatusBar.h>
#include <LibGUI/GTextEditor.h>
#include <LibGUI/GToolBar.h>
#include <LibGUI/GMenuBar.h>
#include <LibGUI/GAction.h>
#include <LibGUI/GLabel.h>
#include <LibGUI/GInputBox.h>
#include <LibGUI/GMessageBox.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include "DirectoryTableView.h"

int main(int argc, char** argv)
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_flags = SA_NOCLDWAIT;
    act.sa_handler = SIG_IGN;
    int rc = sigaction(SIGCHLD, &act, nullptr);
    if (rc < 0) {
        perror("sigaction");
        return 1;
    }

    GApplication app(argc, argv);

    auto* window = new GWindow;
    window->set_title("FileManager");
    window->set_rect(20, 200, 640, 480);
    window->set_should_exit_event_loop_on_close(true);

    auto* widget = new GWidget;
    widget->set_layout(make<GBoxLayout>(Orientation::Vertical));

    auto* main_toolbar = new GToolBar(widget);
    auto* location_toolbar = new GToolBar(widget);
    location_toolbar->layout()->set_margins({ 8, 3, 8, 3 });
    location_toolbar->set_preferred_size({ 0, 21 });

    auto* location_label = new GLabel("Location: ", location_toolbar);
    location_label->size_to_fit();

    auto* location_textbox = new GTextEditor(GTextEditor::SingleLine, location_toolbar);

    auto* directory_view = new DirectoryView(widget);
    auto* statusbar = new GStatusBar(widget);

    location_textbox->on_return_pressed = [directory_view] (auto& editor) {
        directory_view->open(editor.text());
    };

    auto open_parent_directory_action = GAction::create("Open parent directory", { Mod_Alt, Key_Up }, GraphicsBitmap::load_from_file("/res/icons/parentdirectory16.png"), [directory_view] (const GAction&) {
        directory_view->open_parent_directory();
    });

    auto mkdir_action = GAction::create("New directory...", GraphicsBitmap::load_from_file("/res/icons/16x16/mkdir.png"), [&] (const GAction&) {
        GInputBox input_box("Enter name:", "New directory", window);
        if (input_box.exec() == GInputBox::ExecOK && !input_box.text_value().is_empty()) {
            auto new_dir_path = String::format("%s/%s",
                directory_view->path().characters(),
                input_box.text_value().characters()
            );
            int rc = mkdir(new_dir_path.characters(), 0777);
            if (rc < 0) {
                GMessageBox message_box(String::format("mkdir() failed: %s", strerror(errno)), "Error", window);
                message_box.exec();
            } else {
                directory_view->refresh();
            }
        }
    });

    auto view_as_list_action = GAction::create("List view", { Mod_Ctrl, KeyCode::Key_L }, [&] (const GAction&) {
        directory_view->set_view_mode(DirectoryView::ViewMode::List);
    });

    auto view_as_icons_action = GAction::create("Icon view", { Mod_Ctrl, KeyCode::Key_I }, [&] (const GAction&) {
        directory_view->set_view_mode(DirectoryView::ViewMode::Icon);
    });

    auto copy_action = GAction::create("Copy", GraphicsBitmap::load_from_file("/res/icons/copyfile16.png"), [] (const GAction&) {
        dbgprintf("'Copy' action activated!\n");
    });

    auto delete_action = GAction::create("Delete", GraphicsBitmap::load_from_file("/res/icons/16x16/delete.png"), [] (const GAction&) {
        dbgprintf("'Delete' action activated!\n");
    });

    auto menubar = make<GMenuBar>();

    auto app_menu = make<GMenu>("FileManager");
    app_menu->add_action(GAction::create("Quit", { Mod_Alt, Key_F4 }, [] (const GAction&) {
        GApplication::the().quit(0);
        return;
    }));
    menubar->add_menu(move(app_menu));

    auto file_menu = make<GMenu>("File");
    file_menu->add_action(open_parent_directory_action.copy_ref());
    file_menu->add_action(mkdir_action.copy_ref());
    file_menu->add_action(copy_action.copy_ref());
    file_menu->add_action(delete_action.copy_ref());
    menubar->add_menu(move(file_menu));

    auto view_menu = make<GMenu>("View");
    view_menu->add_action(view_as_list_action.copy_ref());
    view_menu->add_action(view_as_icons_action.copy_ref());
    menubar->add_menu(move(view_menu));

    auto help_menu = make<GMenu>("Help");
    help_menu->add_action(GAction::create("About", [] (const GAction&) {
        dbgprintf("FIXME: Implement Help/About\n");
    }));
    menubar->add_menu(move(help_menu));

    app.set_menubar(move(menubar));

    main_toolbar->add_action(open_parent_directory_action.copy_ref());
    main_toolbar->add_action(mkdir_action.copy_ref());
    main_toolbar->add_action(copy_action.copy_ref());
    main_toolbar->add_action(delete_action.copy_ref());

    directory_view->on_path_change = [window, location_textbox] (const String& new_path) {
        window->set_title(String::format("FileManager: %s", new_path.characters()));
        location_textbox->set_text(new_path);
    };

    directory_view->on_status_message = [statusbar] (String message) {
        statusbar->set_text(move(message));
    };

    directory_view->open("/");
    directory_view->set_focus(true);

    window->set_main_widget(widget);
    window->show();

    return app.exec();
}
