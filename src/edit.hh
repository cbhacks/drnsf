//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2020  DRNSF contributors
//
// See the AUTHORS.md file for more details.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#pragma once

/*
 * edit.hh
 *
 * FIXME explain
 */

#include <list>
#include <set>
#include <unordered_map>
#include <typeindex>
#include "../imgui/imgui.h"
#include "res.hh"
#include "transact.hh"
#include "gui.hh"
#include "render.hh"

#include "gfx.hh"
#include "misc.hh"
#include "nsf.hh"

namespace drnsf {
namespace edit {

// defined later in this file
class base_window;

/*
 * edit::context
 *
 * FIXME explain
 */
class context : private util::nocopy {
    friend class base_window;

private:
    // (var) m_proj
    // The project currently open under this context, or null if there is no
    // project currently open.
    std::shared_ptr<res::project> m_proj;

    // (var) m_windows
    // The windows associated with this context. Some of these windows are also
    // owned by the context.
    std::vector<base_window *> m_windows;

public:
    // (default ctor)
    // Creates a context with no project initially open.
    context() = default;

    // (dtor)
    // Destroys the remaining windows owned by the context.
    ~context();

    // (func) get_proj, set_proj
    // Gets or sets the project associated with this context.
    //
    // When setting the project, a copy of the shared pointer to the previous
    // project is kept while delivering the on_project_change event. This ensures
    // that, if this context held the final shared_ptr, the project is kept alive
    // until the event handlers have all been run.
    //
    // It is important that set_proj never be called from inside a transaction,
    // as the project running the transaction could cease to exist during this
    // call if there are no other shared_ptr's to the project remaining.
    const std::shared_ptr<res::project> &get_proj() const;
    void set_proj(std::shared_ptr<res::project> proj);

    // (func) make_window
    // Creates a new window of the specified type. The given parameters are
    // forwarded to the constructor, along with a reference to the context.
    //
    // For example, if called as such:
    //
    //  make_window<foo>(a, b, c);
    //
    // The constructor is called with parameters `a', `b', `c', and `*this'.
    //
    // The specified type must be derived from `base_window'. The window is
    // destroyed when the context is destroyed, or when the `close' method is
    // called on the window (by default this occurs if the user requests that
    // the window close).
    //
    // The window is initially hidden.
    template <typename T, typename... Args>
    T &make_window(Args... args);

    // (event) on_project_change
    // Raised whenever the project is changed by `set_proj'. The previous
    // project, if any, will be kept alive during the execution of this event's
    // handlers, but may be released once the event has finished.
    util::event<const std::shared_ptr<res::project> &> on_project_change;
};

/*
 * edit::base_window
 *
 * FIXME explain
 */
class base_window : public gui::window {
    friend class context;

private:
    // (var) m_owned_by_context
    // True if this window is owned by its associated context, false otherwise.
    bool m_owned_by_context = false;

protected:
    // (var) m_ctx
    // A reference to the context this window is associated with.
    context &m_ctx;

    // (func) on_close_request
    // Implements the close request method for `gui::window'. Closes the window
    // if it is owned by the associated context. Otherwise, no change occurs.
    //
    // Derived types may override this behavior and may or may not call this
    // method implementation.
    void on_close_request() override;

public:
    // (explicit ctor)
    // Constructs the window with the given title and size, and associates it
    // with the given context.
    explicit base_window(
        const std::string &title,
        int width,
        int height,
        context &ctx);

    // (dtor)
    // Destroys the window and removes it from the context.
    virtual ~base_window();

    // (func) close
    // Closes the window and destroys it.
    //
    // This function must only be called on windows which are owned by their
    // associated context (i.e. created by the context's `make_window' method).
    void close();
};

// declared above
template <typename T, typename... Args>
inline T &context::make_window(Args... args)
{
    auto t = new T(std::forward<Args>(args)..., *this);
    t->m_owned_by_context = true;
    return *t;
}

/*
 * edit::mode_handler
 *
 * FIXME explain
 */
class mode_handler : private util::nocopy {
protected:
    // (var) m_ctx
    // A reference to the context which this mode operates on.
    context &m_ctx;

public:
    // (explicit ctor)
    // FIXME explain
    explicit mode_handler(context &ctx) :
        m_ctx(ctx) {}

    // (dtor)
    // Destroys the mode object. This destructor is virtual to allow
    // destruction of derived type objects through a base-class unique_ptr as
    // used in `mode_widget'.
    virtual ~mode_handler() = default;

    // (pure func) start
    // This is called when the owning `mode_widget' switches to this mode.
    virtual void start() = 0;

    // (pure func) stop
    // This is called when the owning `mode_widget' switches away from this
    // mode. When switching between two modes, `stop' is called on the previous
    // mode before `start' is called on the new one.
    virtual void stop() noexcept = 0;
};

/*
 * edit::mode_widget
 *
 * FIXME explain
 */
class mode_widget : private gui::composite {
private:
    // (var) m_ctx
    // A reference to the context which this widget and its modes operate on.
    context &m_ctx;

    // (var) m_modes
    // A map of the mode handler objects for this manager. This map is populated
    // on-demand when switching to new modes.
    std::unordered_map<
        std::type_index,
        std::unique_ptr<mode_handler>> m_modes;

    // (var) m_current_mode
    // A pointer to this window's handler object for the current mode. This is
    // null if no mode is currently active.
    mode_handler *m_current_mode = nullptr;

    // (var) m_current_mode_typeindex
    // The std::type_index of the current mode. This value is only meaningful
    // if `m_current_mode' is non-null.
    std::type_index m_current_mode_typeindex = typeid(void);

protected:
    // (var) m_modeless_content
    // This widget is visible when no mode is currently selected. Derived types
    // may place any relevant UI content here, such as an informational message
    // about no mode being selected. This widget spans the full size of the
    // mode_widget.
    gui::composite m_modeless_content{*this, gui::layout::fill()};

public:
    // (explicit ctor)
    // Constructs the widget with no mode set. The given context is used for
    // all of the modes this widget will contain.
    explicit mode_widget(
        gui::container &parent,
        gui::layout layout,
        context &ctx) :
        composite(parent, layout),
        m_ctx(ctx)
    {
        m_modeless_content.show();
    }

    // (func) is_mode
    // Returns true if the current mode matches the specified mode type.
    template <typename T>
    bool is_mode() const noexcept
    {
        return m_current_mode && m_current_mode_typeindex == typeid(T);
    }

    // (func) set_mode
    // Switches the widget from its current mode to the specified mode. If both
    // modes are the same, no change occurs. If the specified mode is not
    // present in the map, one is constructed.
    //
    // The current mode, if any, receives a `stop' method call before the
    // specified mode receives a `start' call.
    //
    // The destination mode is specified by template type.
    template <typename T>
    void set_mode()
    {
        std::type_index typeindex = typeid(T);

        auto &p = m_modes[typeindex];
        if (!p) {
            p = std::make_unique<T>(
                static_cast<gui::container&>(*this),
                m_ctx
            );
        }

        if (m_current_mode) {
            m_current_mode->stop();
            m_current_mode = nullptr;
        } else {
            m_modeless_content.hide();
        }

        m_current_mode = p.get();
        m_current_mode_typeindex = typeindex;
        try {
            m_current_mode->start();
        } catch (...) {
            m_current_mode = nullptr;
            m_modeless_content.show();
            throw;
        }
    }

    // (func) unset_mode
    // Exits the current mode, if any.
    void unset_mode()
    {
        if (m_current_mode) {
            m_current_mode->stop();
            m_current_mode = nullptr;
            m_modeless_content.show();
        }
    }

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;
};

/*
 * edit::mode_menuset
 *
 * This object provides a set of menu items for each mode available in the
 * editor. When a mode menu item is clicked, the specified mode_widget switches
 * to the associated editor mode.
 */
class mode_menuset {
private:
    // (inner struct) impl
    // Internal implementation details for this class.
    struct impl;

    // (var) M
    // A pointer to the internal implementation object (PIMPL).
    impl *M;

public:
    // (explicit ctor)
    // Constructs the menu items for the various modes and places them in the
    // given menu. When clicked, the menu items direct the specified mode_widget
    // to switch modes.
    explicit mode_menuset(gui::menu &menu, mode_widget &wdg);

    // (dtor)
    // Removes the menu items from the menu.
    ~mode_menuset();
};

namespace menus {

/*
 * edit::menus::mni_open
 *
 * File -> Open
 * Opens an NSF file for the given game.
 */
template <nsf::game_ver GameVersion>
class mni_open : private gui::menu::item {
private:
    context &m_ctx;
    void on_activate() final override;

public:
    explicit mni_open(gui::menu &menu, context &ctx) :
        item(menu,
            GameVersion == nsf::game_ver::crash1 ? "Open (C1)" :
            GameVersion == nsf::game_ver::crash2 ? "Open (C2)" :
            GameVersion == nsf::game_ver::crash3 ? "Open (C3)" :
            "Open (?)"
        ),
        m_ctx(ctx) {}
};

/*
 * edit::menus::mni_save_as
 *
 * File -> Save As
 * Saves the currently open project under a new name.
 */
class mni_save_as : private gui::menu::item {
private:
    context &m_ctx;
    void on_activate() final override;

    decltype(context::on_project_change)::watch h_project_change;

public:
    explicit mni_save_as(gui::menu &menu, context &ctx);
};

/*
 * edit::menus::mni_close
 *
 * File -> Close
 * Closes the currently open project.
 */
class mni_close : private gui::menu::item {
private:
    context &m_ctx;
    void on_activate() final override;

    decltype(context::on_project_change)::watch h_project_change;

public:
    explicit mni_close(gui::menu &menu, context &ctx);
};

/*
 * edit::menus::mni_exit
 *
 * File -> Exit
 * Exits the application.
 */
class mni_exit : private gui::menu::item {
private:
    void on_activate() final override;

public:
    explicit mni_exit(gui::menu &menu) :
        item(menu, "Exit") {}
};

/*
 * edit::menus::mnu_file
 *
 * "File" menu.
 */
class mnu_file : private gui::menubar::item {
private:
    context &m_ctx;
    mni_open<nsf::game_ver::crash1> m_open_c1{*this, m_ctx};
    mni_open<nsf::game_ver::crash2> m_open_c2{*this, m_ctx};
    mni_open<nsf::game_ver::crash3> m_open_c3{*this, m_ctx};
    mni_save_as m_save_as{*this, m_ctx};
    mni_close m_close{*this, m_ctx};
    mni_exit m_exit{*this};

public:
    explicit mnu_file(gui::menubar &menubar, context &ctx) :
        item(menubar, "File"),
        m_ctx(ctx) {}
};

/*
 * edit::menus::mni_undo
 *
 * Edit -> Undo
 * Undoes the last operation.
 */
class mni_undo : private gui::menu::item {
private:
    context &m_ctx;
    void update();
    void on_activate() final override;

    decltype(transact::nexus::on_status_change)::watch h_status_change;
    decltype(context::on_project_change)::watch h_project_change;

public:
    explicit mni_undo(gui::menu &menu, context &ctx);
};

/*
 * edit::menus::mni_redo
 *
 * Edit -> Undo
 * Redoes the last undone operation.
 */
class mni_redo : private gui::menu::item {
private:
    context &m_ctx;
    void update();
    void on_activate() final override;

    decltype(transact::nexus::on_status_change)::watch h_status_change;
    decltype(context::on_project_change)::watch h_project_change;

public:
    explicit mni_redo(gui::menu &menu, context &ctx);
};

/*
 * edit::menus::mnu_edit
 *
 * "Edit" menu.
 */
class mnu_edit : private gui::menubar::item {
private:
    context &m_ctx;
    mode_widget &m_mode_widget;
    mni_undo m_undo{*this, m_ctx};
    mni_redo m_redo{*this, m_ctx};
    mode_menuset m_modes{*this, m_mode_widget};

public:
    explicit mnu_edit(gui::menubar &menubar, context &ctx, mode_widget &wdg) :
        item(menubar, "Edit"),
        m_ctx(ctx),
        m_mode_widget(wdg) {}
};

/*
 * edit::menus::mni_new_window
 *
 * Window -> New Window
 * Spawns a new editor window.
 */
class mni_new_window : private gui::menu::item {
private:
    context &m_ctx;
    void on_activate() final override;

public:
    explicit mni_new_window(gui::menu &menu, context &ctx) :
        item(menu, "New Window"),
        m_ctx(ctx) {}
};

/*
 * edit::menus::mnu_window
 *
 * "Window" menu.
 */
class mnu_window : private gui::menubar::item {
private:
    context &m_ctx;
    mni_new_window m_new_window{*this, m_ctx};

public:
    explicit mnu_window(gui::menubar &menubar, context &ctx) :
        item(menubar, "Window"),
        m_ctx(ctx) {}
};

}

/*
 * edit::field
 *   for all types
 *
 * This type provides an (imgui-only) graphical user interface for editing some
 * kind of data, such as an integer, a color, a polygon, a list or set of data
 * items, etc. Primarily, this is intended for use with the asset property
 * editor control (see `asset_propctl' below), but it could be used elsewhere
 * if necessary.
 *
 * Initially, the field control does not display any data. It must be bound to
 * an existing object first (by pointer). While the field is bound to an object,
 * it displays information about the object to the user. Most implementations
 * of `field' also allow the user to edit the displayed value. This does not
 * modify the object directly, instead an event is raised (`on_change') with
 * the intended new value for the object. The handler of this event is not
 * required to push this change to the object; for example, if the object is
 * currently in a read-only state then this would not be possible.
 *
 * This initial type provides a fallback implementation used when there is no
 * specific `field' implementation for the given type of data.
 */
template <typename T, typename E = void>
class field : private util::nocopy {
private:
    // (var) m_object
    // A pointer to the object this field is bound to, or null if this field is
    // not bound to any object. The field does not directly modify this object,
    // however it may be modified by external code during the lifetime of the
    // binding.
    const T *m_object;

public:
    // (func) bind
    // If a non-null pointer is passed, binds the field to the given object.
    // The existing binding is discarded, if any. The lifetime of the binding
    // must not extend beyond the lifetime of the object.
    //
    // If a null pointer is passed, discards the existing binding, if any.
    void bind(const T *object)
    {
        m_object = object;
    }

    // (func) frame
    // FIXME explain
    void frame()
    {
        if (!m_object) {
            ImGui::Text("--");
            return;
        }
        auto &obj = *m_object;

        (void)obj;
        ImGui::Text("No options available.");
    }

    // (event) on_change
    // This event is raised when the user attempts to change the value of the
    // field. The argument given is the new value given by the user.
    util::event<T> on_change;
};

/*
 * edit::field
 *   for integral types
 *
 * This provides a specialized version of `edit::field' for integral types,
 * such as `int', `short', `unsigned long long', `uint8_t', etc.
 *
 * For more details, see the non-specialized version of `edit::field'.
 */
template <typename T>
class field<T, std::enable_if_t<std::is_integral<T>::value>> :
    private util::nocopy {
private:
    // (var) m_object
    // See the non-specialized `edit::field' for details.
    const T *m_object;

public:
    // (func) bind
    // See the non-specialized `edit::field' for details.
    void bind(const T *object)
    {
        m_object = object;
    }

    // (func) frame
    // See the non-specialized `edit::field' for details.
    void frame()
    {
        if (!m_object) {
            ImGui::Text("--");
            return;
        }
        auto &obj = *m_object;

        intmax_t min = std::numeric_limits<T>::min();
        uintmax_t max = std::numeric_limits<T>::max();

        intmax_t min64 = std::numeric_limits<ImS64>::min();
        uintmax_t max64 = std::numeric_limits<ImS64>::max();

        if (min >= INT_MIN && max <= INT_MAX && max - min <= 255) {
            // If in the range of int and the range of values is 256 or less
            // (int8_t, uint8_t, etc) then use a slider input.

            int value = obj;
            if (ImGui::SliderInt("", &value, min, max)) {
                on_change(value);
            }
        } else if (min >= min64 && max <= max64) {
            // If within the range of ImS64 (should be 64-bit), use an ImS64
            // input field. This should fit all integral types used for asset
            // properties.

            ImS64 value = obj;
            if (ImGui::InputScalar("", ImGuiDataType_S64, &value)) {
                if (value < ImS64(min)) {
                    value = min;
                    ImGui::SameLine();
                    ImGui::Text("out of range; too low");
                } else if (value > ImS64(max)) {
                    value = max;
                    ImGui::SameLine();
                    ImGui::Text("out of range; too high");
                }
                if (value != obj) {
                    on_change(value);
                }
            }
        } else {
            using std::to_string;
            ImGui::TextUnformatted(to_string(obj).c_str());
        }
    }

    // (event) on_change
    // See the non-specialized `edit::field' for details.
    util::event<T> on_change;
};

/*
 * edit::field
 *   for floating point types
 *
 * This provides a specialized version of `edit::field' for floating point
 * types, such as `float' and `double'.
 *
 * For more details, see the non-specialized version of `edit::field'.
 */
template <typename T>
class field<T, std::enable_if_t<std::is_floating_point<T>::value>> :
    private util::nocopy {
private:
    // (var) m_object
    // See the non-specialized `edit::field' for details.
    const T *m_object;

public:
    // (func) bind
    // See the non-specialized `edit::field' for details.
    void bind(const T *object)
    {
        m_object = object;
    }

    // (func) frame
    // See the non-specialized `edit::field' for details.
    void frame()
    {
        if (!m_object) {
            ImGui::Text("--");
            return;
        }
        auto &obj = *m_object;

        if (typeid(T) == typeid(float)) {
            float value = obj;
            if (ImGui::InputFloat("", &value)) {
                on_change(value);
            }
        } else if (typeid(T) == typeid(double)) {
            double value = obj;
            if (ImGui::InputDouble("", &value)) {
                on_change(value);
            }
        } else {
            using std::to_string;
            ImGui::TextUnformatted(to_string(obj).c_str());
        }
    }

    // (event) on_change
    // See the non-specialized `edit::field' for details.
    util::event<T> on_change;
};

/*
 * edit::field
 *   for entry ID's
 *
 * This provides a specialized version of `edit::field' for `nsf::eid'.
 *
 * For more details, see the non-specialized version of `edit::field'.
 */
template <>
class field<nsf::eid> : private util::nocopy {
private:
    // (var) m_object
    // See the non-specialized `edit::field' for details.
    const nsf::eid *m_object;

public:
    // (func) bind
    // See the non-specialized `edit::field' for details.
    void bind(const nsf::eid *object)
    {
        m_object = object;
    }

    // (func) frame
    // See the non-specialized `edit::field' for details.
    void frame()
    {
        if (!m_object) {
            ImGui::Text("--");
            return;
        }
        auto &obj = *m_object;

        bool is_valid = obj.is_valid();
        if (!is_valid) {
            ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
        }

        char buf[8];
        std::strcpy(buf, obj.str().c_str());
        if (ImGui::InputText("", buf, sizeof(buf))) {
            auto new_value = obj;
            if (new_value.try_parse(buf)) {
                if (new_value != obj) {
                    on_change(new_value);
                }
            } else {
                ImGui::SameLine();
                ImGui::Text("parse error");
            }
        }

        if (!is_valid) {
            ImGui::PopStyleColor();
        }
    }

    // (event) on_change
    // See the non-specialized `edit::field' for details.
    util::event<nsf::eid> on_change;
};

/*
 * edit::field
 *   for asset names (atoms)
 *
 * This provides a specialized version of `edit::field' for `res::atom', as
 * used for asset names and in asset references.
 *
 * For more details, see the non-specialized version of `edit::field'.
 */
template <>
class field<res::atom> : private util::nocopy {
private:
    // (var) m_object
    // See the non-specialized `edit::field' for details.
    const res::atom *m_object;

public:
    // (func) bind
    // See the non-specialized `edit::field' for details.
    void bind(const res::atom *object)
    {
        m_object = object;
    }

    // (func) frame
    // See the non-specialized `edit::field' for details.
    void frame()
    {
        if (!m_object) {
            ImGui::Text("--");
            return;
        }
        auto &obj = *m_object;

        ImGui::TextUnformatted(to_string(obj).c_str());
    }

    // (event) on_change
    // See the non-specialized `edit::field' for details.
    util::event<res::atom> on_change;
};

/*
 * edit::field
 *   for asset references
 *
 * This provides a specialized version of `edit::field' for `res::ref<T>'.
 *
 * For more details, see the non-specialized version of `edit::field'.
 */
template <typename T>
class field<res::ref<T>> : private field<res::atom> {
private:
    // (var) m_object
    // See the non-specialized `edit::field' for details.
    const res::ref<T> *m_object;

public:
    // (default ctor)
    // Installs an event handler onto the internal `res::atom' field change
    // event to propagate the change to the outer even defined below.
    field()
    {
        field<res::atom>::on_change <<= [this](res::atom new_value) {
            on_change(new_value);
        };
    }

    // (func) bind
    // See the non-specialized `edit::field' for details.
    void bind(const res::ref<T> *object)
    {
        m_object = object;
        field<res::atom>::bind(object);
    }

    // (func) frame
    // See the non-specialized `edit::field' for details.
    void frame()
    {
        if (!m_object) {
            ImGui::Text("--");
            return;
        }
        auto &obj = *m_object;

        // Display the basic atom field contents.
        field<res::atom>::frame();

        // On the same line, display information about the status of the name
        // in relation to `T', such as OK, not present, type mismatch, etc.
        //
        // Display nothing if there is the value is a null reference.
        if (obj) {
            ImGui::SameLine();
            if (obj.template is_a<T>()) {
                const ImVec4 color = { 0.0f, 0.5f, 0.0f, 1.0f };
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::Text("OK");
                ImGui::PopStyleColor();
            } else if (obj.template is_a<res::asset>()) {
                const ImVec4 color = { 0.5f, 0.25f, 0.0f, 1.0f };
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::Text("Wrong asset type");
                ImGui::PopStyleColor();
            } else {
                const ImVec4 color = { 0.5f, 0.0f, 0.0f, 1.0f };
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::Text("Doesn't exist");
                ImGui::PopStyleColor();
            }
        }
    }

    // (event) on_change
    // See the non-specialized `edit::field' for details.
    util::event<res::ref<T>> on_change;
};

/*
 * edit::field
 *   for lists (std::vector) of any type
 *
 * This provides a specialized version of `edit::field' for `std::vector<T>'
 * where T is some other type. The user is presented with an `edit::field<T>'
 * and can flip through the list to choose which element to edit in the field.
 * The user may also modify the list by inserting or removing elements.
 *
 * For more details, see the non-specialized version of `edit::field'.
 */
template <typename T>
class field<std::vector<T>> : private util::nocopy {
private:
    // (var) m_object
    // See the non-specialized `edit::field' for details.
    const std::vector<T> *m_object;

    // (var) m_elem_field
    // The `edit::field' which displays information about the selected element
    // in the list.
    field<T> m_elem_field;

    // (var) m_index
    // The index of the currently selected element in the list. The value is
    // preserved while in an unbound state, but is not otherwise meaningful at
    // that time.
    unsigned int m_index = 0;

public:
    // (default ctor)
    // Sets up an event handler for the inner element field to propagate any
    // changes to the event on the outer field (this).
    field()
    {
        m_elem_field.on_change <<= [this](T new_elem_value) {
            auto new_list = *m_object;

            new_list[m_index] = std::move(new_elem_value);

            on_change(std::move(new_list));
        };
    }

    // (func) bind
    // See the non-specialized `edit::field' for details.
    void bind(const std::vector<T> *object)
    {
        m_object = object;

        // Also apply the binding to the inner field.
        if (m_object && !m_object->empty()) {
            // If the current index is beyond the range of the new list, change
            // it to point at the final element.
            if (m_index >= m_object->size()) {
                m_index = m_object->size() - 1;
            }
            m_elem_field.bind(&m_object->operator [](m_index));
        } else {
            m_elem_field.bind(nullptr);
        }
    }

    // (func) frame
    // See the non-specialized `edit::field' for details.
    void frame()
    {
        if (!m_object) {
            ImGui::Text("--");
            ImGui::NextColumn();
            ImGui::Indent();
            ImGui::Text("--");
            ImGui::NextColumn();
            m_elem_field.frame();
            ImGui::NextColumn();
            ImGui::Unindent();
            ImGui::NextColumn();
            return;
        }
        auto &obj = *m_object;

        ImGui::TextUnformatted("List of $"_fmt(obj.size()).c_str());

        // On the next row, display inputs for seeking through the list.
        if (!obj.empty()) {
            // First button to go to the first element.
            if (ImGui::Button("|<")) {
                if (!obj.empty()) {
                    m_index = 0;
                    m_elem_field.bind(&obj[m_index]);
                }
            }

            // Previous button to decrease element index.
            ImGui::SameLine();
            if (ImGui::Button("<")) {
                if (m_index > 0) {
                    m_index--;
                    m_elem_field.bind(&obj[m_index]);
                }
            }

            // A slider to allow the user to quickly seek through the list.
            ImGui::SameLine();
            int index_s = int(m_index);
            ImGui::SliderInt("###index", &index_s, 0, obj.size() - 1);
            if (m_index != (unsigned int)index_s) {
                m_index = (unsigned int)index_s;
                if (m_index < obj.size()) {
                    m_elem_field.bind(&obj[m_index]);
                } else {
                    m_elem_field.bind(nullptr);
                }
            }

            // Next button to increase element index.
            ImGui::SameLine();
            if (ImGui::Button(">")) {
                if (!obj.empty() && m_index < obj.size() - 1) {
                    m_index++;
                    m_elem_field.bind(&obj[m_index]);
                }
            }

            // Last button to go to the final element.
            ImGui::SameLine();
            if (ImGui::Button(">|")) {
                if (!obj.empty()) {
                    m_index = obj.size() - 1;
                    m_elem_field.bind(&obj[m_index]);
                }
            }

            // Insert / remove / etc buttons on the next line.
            if (ImGui::SmallButton("Append")) {
                auto old_size = obj.size();

                auto new_list = obj;
                new_list.push_back(obj.back());
                on_change(std::move(new_list));

                // Jump to the new element if the append appears to have gone
                // through.
                if (obj.size() == old_size + 1) {
                    m_index = old_size;
                }
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("Insert")) {
                if (m_index < obj.size()) {
                    auto new_list = obj;
                    new_list.insert(new_list.begin() + m_index, obj[m_index]);
                    on_change(std::move(new_list));
                }
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("Remove")) {
                if (m_index < obj.size()) {
                    auto new_list = obj;
                    new_list.erase(new_list.begin() + m_index);
                    on_change(std::move(new_list));
                }
            }
        } else {
            ImGui::Text("--");

            if (ImGui::SmallButton("Append")) {
                auto new_list = obj;
                new_list.emplace_back();
                on_change(std::move(new_list));
            }
        }

        // Display the inner field and zero-based index on the next row.
        ImGui::NextColumn();
        ImGui::Indent();
        ImGui::TextUnformatted("[$]"_fmt(m_index).c_str());
        ImGui::NextColumn();
        ImGui::PushID(m_index);
        m_elem_field.frame();
        ImGui::PopID();
        ImGui::NextColumn();
        ImGui::Unindent();
        ImGui::NextColumn();
    }

    // (event) on_change
    // See the non-specialized `edit::field' for details.
    util::event<std::vector<T>> on_change;
};

/*
 * edit::field
 *   for raw data (util::blob)
 *
 * This provides a specialized version of `edit::field' for `util::blob'. This
 * field is presented as a basic hex editor.
 *
 * For more details, see the non-specialized version of `edit::field'.
 */
template <>
class field<util::blob> : private util::nocopy {
private:
    // (var) m_object
    // See the non-specialized `edit::field' for details.
    const util::blob *m_object;

    // (var) m_selected_byte
    // The offset of the currently selected byte in the editor. This may be
    // out of range to indicate that no byte is currently selected.
    size_t m_selected_byte = SIZE_MAX;

    // (var) m_input_value
    // The value currently being input by the user.
    uint8_t m_input_value;

    // (var) m_input_len
    // The number of hex characters present in `m_input_value'. This increases
    // by one each time the user enters a hex digit. When the value reaches 2,
    // it is reset and the value is written to the selected byte location.
    int m_input_len = 0;

public:
    // (func) bind
    // See the non-specialized `edit::field' for details.
    void bind(const util::blob *object)
    {
        m_object = object;
    }

    // (func) frame
    // See the non-specialized `edit::field' for details.
    void frame();

    // (event) on_change
    // See the non-specialized `edit::field' for details.
    util::event<util::blob> on_change;
};

/*
 * edit::field
 *   for gfx::vertex
 *
 * This provides a specialized version of `edit::field' for `gfx::vertex'. The
 * field breaks down the structure and provides a field for each of its member
 * values.
 *
 * For more details, see the non-specialized version of `edit::field'.
 */
template <>
class field<gfx::vertex> : private util::nocopy {
private:
    // (var) m_object
    // See the non-specialized `edit::field' for details.
    const gfx::vertex *m_object;

    // (var) m_x_field, m_y_field, m_z_field
    // The subfields for X/Y/Z vertex components.
    field<int> m_x_field;
    field<int> m_y_field;
    field<int> m_z_field;

    // (var) m_fx_field
    // The subfield for the vertex special effects bits.
    field<int> m_fx_field;

    // (var) m_color_index_field
    // The subfield for the vertex's color index.
    field<int> m_color_index_field;

public:
    // (default ctor)
    // Sets up event handlers for the subfields to propagate any changes to
    // the event on the outer field (this).
    field()
    {
        m_x_field.on_change <<= [this](int new_value) {
            auto new_vtx = *m_object;
            new_vtx.x = new_value;
            on_change(new_vtx);
        };
        m_y_field.on_change <<= [this](int new_value) {
            auto new_vtx = *m_object;
            new_vtx.y = new_value;
            on_change(new_vtx);
        };
        m_z_field.on_change <<= [this](int new_value) {
            auto new_vtx = *m_object;
            new_vtx.z = new_value;
            on_change(new_vtx);
        };
        m_fx_field.on_change <<= [this](int new_value) {
            auto new_vtx = *m_object;
            new_vtx.fx = new_value;
            on_change(new_vtx);
        };
        m_color_index_field.on_change <<= [this](int new_value) {
            auto new_vtx = *m_object;
            new_vtx.color_index = new_value;
            on_change(new_vtx);
        };
    }

    // (func) bind
    // See the non-specialized `edit::field' for details.
    void bind(const gfx::vertex *object)
    {
        m_object = object;

        // Also apply the binding to the subfields.
        if (object) {
            m_x_field.bind(&object->x);
            m_y_field.bind(&object->y);
            m_z_field.bind(&object->z);
            m_fx_field.bind(&object->fx);
            m_color_index_field.bind(&object->color_index);
        } else {
            m_x_field.bind(nullptr);
            m_y_field.bind(nullptr);
            m_z_field.bind(nullptr);
            m_fx_field.bind(nullptr);
            m_color_index_field.bind(nullptr);
        }
    }

    // (func) frame
    // See the non-specialized `edit::field' for details.
    void frame()
    {
        ImGui::NextColumn();
        ImGui::Indent();

        ImGui::Text("X");
        ImGui::NextColumn();
        ImGui::PushID(0);
        m_x_field.frame();
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::Text("Y");
        ImGui::NextColumn();
        ImGui::PushID(1);
        m_y_field.frame();
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::Text("Z");
        ImGui::NextColumn();
        ImGui::PushID(2);
        m_z_field.frame();
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::Text("FX");
        ImGui::NextColumn();
        ImGui::PushID(3);
        m_fx_field.frame();
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::Text("Color Index");
        ImGui::NextColumn();
        ImGui::PushID(4);
        m_color_index_field.frame();
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::Unindent();
        ImGui::NextColumn();
    }

    // (event) on_change
    // See the non-specialized `edit::field' for details.
    util::event<gfx::vertex> on_change;
};

/*
 * edit::field
 *   for gfx::rgb888
 *
 * This provides a specialized version of `edit::field' for `gfx::rgb888'. The
 * field provides sliders for each of the three color channels (RGB) and
 * displays the color to the user.
 *
 * For more details, see the non-specialized version of `edit::field'.
 */
template <>
class field<gfx::rgb888> : private util::nocopy {
private:
    // (var) m_object
    // See the non-specialized `edit::field' for details.
    const gfx::rgb888 *m_object;

public:
    // (func) bind
    // See the non-specialized `edit::field' for details.
    void bind(const gfx::rgb888 *object)
    {
        m_object = object;
    }

    // (func) frame
    // See the non-specialized `edit::field' for details.
    void frame()
    {
        if (!m_object) {
            ImGui::NextColumn();
            ImGui::Indent();

            ImGui::Text("Red");
            ImGui::NextColumn();
            ImGui::Text("--");
            ImGui::NextColumn();

            ImGui::Text("Green");
            ImGui::NextColumn();
            ImGui::Text("--");
            ImGui::NextColumn();

            ImGui::Text("Blue");
            ImGui::NextColumn();
            ImGui::Text("--");
            ImGui::NextColumn();

            ImGui::Unindent();
            ImGui::NextColumn();
            return;
        }
        auto &obj = *m_object;

        ImGui::NextColumn();
        ImGui::Indent();
        ImVec4 im_color_bg = {
            obj.r / 255.0f,
            obj.g / 255.0f,
            obj.b / 255.0f,
            1.0
        };
        ImVec4 im_color_fg1;
        ImVec4 im_color_fg2;
        if ((obj.r + obj.g + obj.b) / 3 < 100) {
            im_color_fg1 = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
            im_color_fg2 = ImVec4{0.8f, 0.8f, 0.8f, 1.0f};
        } else {
            im_color_fg1 = ImVec4{0.0f, 0.0f, 0.0f, 1.0f};
            im_color_fg2 = ImVec4{0.2f, 0.2f, 0.2f, 1.0f};
        }
        ImGui::PushStyleColor(ImGuiCol_FrameBg, im_color_bg);
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, im_color_fg2);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, im_color_fg2);

        for (int i = 0; i < 3; i++) {
            const char *labels[] = { "Red", "Green", "Blue" };
            ImGui::TextUnformatted(labels[i]);
            ImGui::NextColumn();
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Text, im_color_fg1);
            int n = obj.v[i];
            if (ImGui::SliderInt("", &n, 0, 255)) {
                auto new_value = obj;
                new_value.v[i] = n;
                on_change(new_value);
            }
            ImGui::PopStyleColor();
            ImGui::PopID();
            ImGui::NextColumn();
        }

        ImGui::PopStyleColor(3);
        ImGui::Unindent();
        ImGui::NextColumn();
    }

    // (event) on_change
    // See the non-specialized `edit::field' for details.
    util::event<gfx::rgb888> on_change;
};

/*
 * edit::field
 *   for gfx::corner
 *
 * This provides a specialized version of `edit::field' for `gfx::corner'. The
 * field breaks down the structure and provides a field for each of its member
 * values.
 *
 * For more details, see the non-specialized version of `edit::field'.
 */
template <>
class field<gfx::corner> : private util::nocopy {
private:
    // (var) m_object
    // See the non-specialized `edit::field' for details.
    const gfx::corner *m_object;

    // (var) m_vertex_field, m_color_field
    // The subfields for the corner's components.
    field<int> m_vertex_field;
    field<int> m_color_field;

public:
    // (default ctor)
    // Sets up event handlers for the subfields to propagate any changes to
    // the event on the outer field (this).
    field()
    {
        m_vertex_field.on_change <<= [this](int new_value) {
            auto new_cnr = *m_object;
            new_cnr.vertex_index = new_value;
            on_change(new_cnr);
        };
        m_color_field.on_change <<= [this](int new_value) {
            auto new_cnr = *m_object;
            new_cnr.color_index = new_value;
            on_change(new_cnr);
        };
    }

    // (func) bind
    // See the non-specialized `edit::field' for details.
    void bind(const gfx::corner *object)
    {
        m_object = object;

        // Also apply the binding to the subfields.
        if (object) {
            m_vertex_field.bind(&object->vertex_index);
            m_color_field.bind(&object->color_index);
        } else {
            m_vertex_field.bind(nullptr);
            m_color_field.bind(nullptr);
        }
    }

    // (func) frame
    // See the non-specialized `edit::field' for details.
    void frame()
    {
        ImGui::NextColumn();
        ImGui::Indent();

        ImGui::Text("Vertex Index");
        ImGui::NextColumn();
        ImGui::PushID(0);
        m_vertex_field.frame();
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::Text("Color Index");
        ImGui::NextColumn();
        ImGui::PushID(1);
        m_color_field.frame();
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::Unindent();
        ImGui::NextColumn();
    }

    // (event) on_change
    // See the non-specialized `edit::field' for details.
    util::event<gfx::corner> on_change;
};

/*
 * edit::field
 *   for gfx::triangle
 *
 * This provides a specialized version of `edit::field' for `gfx::triangle'.
 * The field breaks down the structure and provides a field for each of its
 * member values.
 *
 * For more details, see the non-specialized version of `edit::field'.
 */
template <>
class field<gfx::triangle> : private util::nocopy {
private:
    // (var) m_object
    // See the non-specialized `edit::field' for details.
    const gfx::triangle *m_object;

    // (var) m_corner_fields
    // The subfields for the three corner components.
    field<gfx::corner> m_corner_fields[3];

    // (var) m_unk0_field, m_unk1_field
    // The subfields for the unknown triangle components. Most likely these
    // should represent a material index.
    field<unsigned int> m_unk0_field;
    field<unsigned int> m_unk1_field;

public:
    // (default ctor)
    // Sets up event handlers for the subfields to propagate any changes to
    // the event on the outer field (this).
    field()
    {
        for (int i = 0; i < 3; i++) {
            m_corner_fields[i].on_change <<= [this, i](gfx::corner new_value) {
                auto new_tri = *m_object;
                new_tri.v[i] = new_value;
                on_change(new_tri);
            };
        }
        m_unk0_field.on_change <<= [this](unsigned int new_value) {
            auto new_tri = *m_object;
            new_tri.unk0 = new_value;
            on_change(new_tri);
        };
        m_unk1_field.on_change <<= [this](unsigned int new_value) {
            auto new_tri = *m_object;
            new_tri.unk1 = new_value;
            on_change(new_tri);
        };
    }

    // (func) bind
    // See the non-specialized `edit::field' for details.
    void bind(const gfx::triangle *object)
    {
        m_object = object;

        // Also apply the binding to the subfields.
        if (object) {
            for (int i = 0; i < 3; i++) {
                m_corner_fields[i].bind(&object->v[i]);
            }
            m_unk0_field.bind(&object->unk0);
            m_unk1_field.bind(&object->unk1);
        } else {
            for (int i = 0; i < 3; i++) {
                m_corner_fields[i].bind(nullptr);
            }
            m_unk0_field.bind(nullptr);
            m_unk1_field.bind(nullptr);
        }
    }

    // (func) frame
    // See the non-specialized `edit::field' for details.
    void frame()
    {
        ImGui::NextColumn();
        ImGui::Indent();

        const char *corner_labels[] = {
            "Corner A",
            "Corner B",
            "Corner C"
        };
        for (int i = 0; i < 3; i++) {
            ImGui::TextUnformatted(corner_labels[i]);
            ImGui::NextColumn();
            ImGui::PushID(i);
            m_corner_fields[i].frame();
            ImGui::PopID();
            ImGui::NextColumn();
        }

        ImGui::Text("Unknown 0");
        ImGui::NextColumn();
        ImGui::PushID(3);
        m_unk0_field.frame();
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::Text("Unknown 1");
        ImGui::NextColumn();
        ImGui::PushID(4);
        m_unk1_field.frame();
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::Unindent();
        ImGui::NextColumn();
    }

    // (event) on_change
    // See the non-specialized `edit::field' for details.
    util::event<gfx::triangle> on_change;
};

/*
 * edit::field
 *   for gfx::quad
 *
 * This provides a specialized version of `edit::field' for `gfx::quad'. The
 * field breaks down the structure and provides a field for each of its member
 * values.
 *
 * For more details, see the non-specialized version of `edit::field'.
 */
template <>
class field<gfx::quad> : private util::nocopy {
private:
    // (var) m_object
    // See the non-specialized `edit::field' for details.
    const gfx::quad *m_object;

    // (var) m_corner_fields
    // The subfields for the four corner components.
    field<gfx::corner> m_corner_fields[4];

    // (var) m_unk0_field, m_unk1_field
    // The subfields for the unknown quad components. Most likely these should
    // represent a material index.
    field<unsigned int> m_unk0_field;
    field<unsigned int> m_unk1_field;

public:
    // (default ctor)
    // Sets up event handlers for the subfields to propagate any changes to
    // the event on the outer field (this).
    field()
    {
        for (int i = 0; i < 4; i++) {
            m_corner_fields[i].on_change <<= [this, i](gfx::corner new_value) {
                auto new_quad = *m_object;
                new_quad.v[i] = new_value;
                on_change(new_quad);
            };
        }
        m_unk0_field.on_change <<= [this](unsigned int new_value) {
            auto new_quad = *m_object;
            new_quad.unk0 = new_value;
            on_change(new_quad);
        };
        m_unk1_field.on_change <<= [this](unsigned int new_value) {
            auto new_quad = *m_object;
            new_quad.unk1 = new_value;
            on_change(new_quad);
        };
    }

    // (func) bind
    // See the non-specialized `edit::field' for details.
    void bind(const gfx::quad *object)
    {
        m_object = object;

        // Also apply the binding to the subfields.
        if (object) {
            for (int i = 0; i < 4; i++) {
                m_corner_fields[i].bind(&object->v[i]);
            }
            m_unk0_field.bind(&object->unk0);
            m_unk1_field.bind(&object->unk1);
        } else {
            for (int i = 0; i < 4; i++) {
                m_corner_fields[i].bind(nullptr);
            }
            m_unk0_field.bind(nullptr);
            m_unk1_field.bind(nullptr);
        }
    }

    // (func) frame
    // See the non-specialized `edit::field' for details.
    void frame()
    {
        ImGui::NextColumn();
        ImGui::Indent();

        const char *corner_labels[] = {
            "Corner A",
            "Corner B",
            "Corner C",
            "Corner D"
        };
        for (int i = 0; i < 4; i++) {
            ImGui::TextUnformatted(corner_labels[i]);
            ImGui::NextColumn();
            ImGui::PushID(i);
            m_corner_fields[i].frame();
            ImGui::PopID();
            ImGui::NextColumn();
        }

        ImGui::Text("Unknown 0");
        ImGui::NextColumn();
        ImGui::PushID(4);
        m_unk0_field.frame();
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::Text("Unknown 1");
        ImGui::NextColumn();
        ImGui::PushID(5);
        m_unk1_field.frame();
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::Unindent();
        ImGui::NextColumn();
    }

    // (event) on_change
    // See the non-specialized `edit::field' for details.
    util::event<gfx::quad> on_change;
};

/*
 * edit::asset_metactl
 *
 * This widget displays basic information about an asset, such as its name and
 * type, and provides inputs for some basic operations which can be applied to
 * it, such as deletion, renaming, etc.
 *
 * The asset is selected by name, and the control watches the name's associated
 * project to update itself when the named asset appears or disappears from the
 * project.
 */
class asset_metactl : private gui::widget_im {
private:
    // (var) m_name
    // FIXME explain
    res::atom m_name;

    // (handler) h_asset_appear, h_asset_disappear
    // Hooks the project's on_asset_appear and on_asset_disappear events so
    // that, if the selected asset comes into or out of existence, the widget
    // can be updated.
    decltype(res::project::on_asset_appear)::watch h_asset_appear;
    decltype(res::project::on_asset_disappear)::watch h_asset_disappear;

    // (func) frame
    // Implements `gui::widget_im::frame' to enact the widget's contents using
    // ImGui.
    void frame() override;

public:
    // (explicit ctor)
    // Constructs the metactl widget in the given container with the given
    // layout. By default, it is not set to use any asset name.
    explicit asset_metactl(gui::container &parent, gui::layout layout);

    // (func) set_name
    // FIXME explain
    void set_name(res::atom name);

    using widget_im::show;
    using widget_im::hide;
    using widget_im::get_layout;
    using widget_im::set_layout;
    using widget_im::get_real_size;
    using widget_im::get_screen_pos;
};

/*
 * edit::asset_viewctl
 *
 * This widget displays basic information about an asset, such as its name and
 * type, and provides inputs for some basic operations which can be applied to
 * it, such as deletion, renaming, etc.
 *
 * The asset is selected by name, and the control watches the name's associated
 * project to update itself when the named asset appears or disappears from the
 * project.
 */
class asset_viewctl : private gui::composite {
private:
    // (var) m_name
    // FIXME explain
    res::atom m_name;

    // (var) m_inner
    // A pointer to a specialized internal widget which handles the specific
    // type of the view's asset.
    std::unique_ptr<util::polymorphic> m_inner;

    // (handler) h_asset_appear, h_asset_disappear
    // Hooks the project's on_asset_appear and on_asset_disappear events so
    // that, if the selected asset comes into or out of existence, the widget
    // can be updated.
    decltype(res::project::on_asset_appear)::watch h_asset_appear;
    decltype(res::project::on_asset_disappear)::watch h_asset_disappear;

public:
    // (explicit ctor)
    // Constructs the viewctl widget in the given container with the given
    // layout. By default, it is not set to use any asset name.
    explicit asset_viewctl(gui::container &parent, gui::layout layout);

    // (func) set_name
    // FIXME explain
    void set_name(res::atom name);

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;
};

/*
 * edit::asset_propctl
 *
 * This widget displays the properties of an asset and allows the user to
 * modify their values.
 */
class asset_propctl : private gui::widget_im {
private:
    // (var) m_name
    // FIXME explain
    res::atom m_name;

    // (var) m_inner
    // A pointer to a specialized internal widget which handles the specific
    // type of the selected asset.
    std::unique_ptr<util::polymorphic> m_inner;

    // (handler) h_asset_appear, h_asset_disappear
    // Hooks the project's on_asset_appear and on_asset_disappear events so
    // that, if the selected asset comes into or out of existence, the widget
    // can be updated.
    decltype(res::project::on_asset_appear)::watch h_asset_appear;
    decltype(res::project::on_asset_disappear)::watch h_asset_disappear;

    // (func) frame
    // Implements `gui::widget_im::frame' to enact the widget's contents using
    // ImGui.
    void frame() override;

public:
    // (explicit ctor)
    // Constructs the propctl widget in the given container with the given
    // layout. By default, it is not set to use any asset name.
    explicit asset_propctl(gui::container &parent, gui::layout layout);

    // (func) set_name
    // FIXME explain
    void set_name(res::atom name);

    using widget_im::show;
    using widget_im::hide;
    using widget_im::get_layout;
    using widget_im::set_layout;
    using widget_im::get_real_size;
    using widget_im::get_screen_pos;
};

/*
 * edit::asset_mainctl
 *
 * This widget displays an asset to the user and provides the ability to change
 * or manipulate the asset in some way
 *
 * This is a composite of several different asset widgets defined above.
 */
class asset_mainctl : private gui::composite {
private:
    // (var) m_metactl
    // An instance of the asset_metactl placed roughly in the top-left quarter
    // of the widget.
    asset_metactl m_metactl{*this, gui::layout::grid(0, 3, 8, 0, 1, 2)};

    // (var) m_viewctl
    // An instance of the asset_viewctl placed roughly in the top-right quarter
    // of the widget.
    asset_viewctl m_viewctl{*this, gui::layout::grid(3, 5, 8, 0, 1, 2)};

    // (var) m_propctl
    // An instance of the asset_propctl placed roughly in the bottom half of
    // the widget.
    asset_propctl m_propctl{*this, gui::layout::grid(0, 1, 1, 1, 1, 2)};

public:
    // (explicit ctor)
    // Constructs the widget in the given container with the given layout. The
    // initial asset name is null, so the widget will merely show generic "no
    // asset selected" messages.
    explicit asset_mainctl(gui::container &parent, gui::layout layout) :
        composite(parent, layout)
    {
        m_metactl.show();
        m_viewctl.show();
        m_propctl.show();
    }

    // (func) set_name
    // Sets the asset name used by all of the contained widgets.
    void set_name(res::atom name)
    {
        m_metactl.set_name(name);
        m_viewctl.set_name(name);
        m_propctl.set_name(name);
    }

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;
};

/*
 * edit::asset_tree
 *
 * FIXME explain
 */
class asset_tree : private gui::composite {
private:
    // inner class defined in edit_asset_tree.cc
    class impl;

    // (var) M
    // The pointer to the internal implementation object (PIMPL).
    impl *M;

public:
    // (ctor)
    // Constructs the widget and places it in the given parent container.
    asset_tree(gui::container &parent, gui::layout layout);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~asset_tree();

    // (func) get_base, set_base
    // Gets or sets the base of the tree which the widget displays. If this is
    // a root name (i.e. a project root), the entire tree under that root is
    // displayed. If the name is null, nothing is displayed.
    //
    // The specified base itself does not appear in the tree, and the user
    // cannot select it.
    //
    // When the base is changed, the node selection is cleared.
    const res::atom &get_base() const;
    void set_base(res::atom base);

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;

    // (event) on_select
    // Raised when the selected asset has changed. Note that this can be
    // triggered automatically, for example if the currently selected asset
    // vanishes and the tree automatically selects another list item.
    util::event<res::atom> on_select;
};

/*
 * See other files for individual editor modes:
 *
 *  - Classic mode
 *    edit_mode_classic.hh (edit::mode_classic)
 *
 *  - Assets mode
 *    edit_mode_assets.hh (edit::mode_assets)
 *
 *  - Map mode
 *    edit_mode_map.hh (edit::mode_map)
 */

/*
 * edit::mode_window
 *
 * FIXME explain
 */
class mode_window : public base_window, private mode_widget {
private:
    gui::menubar m_menubar{*this};
    menus::mnu_edit m_mnu_edit{m_menubar, base_window::m_ctx, *this};

    // (var) m_modeless_message
    // An informational message which appears when no mode is selected.
    gui::label m_modeless_message {m_modeless_content, gui::layout::fill()};

public:
    // (explicit ctor)
    // Constructs the window and associates it with the given context. No mode
    // is initially selected in the window.
    explicit mode_window(context &ctx);

    using base_window::show;
    using mode_widget::is_mode;
    using mode_widget::set_mode;
    using mode_widget::unset_mode;
};

// FIXME FIXME FIXME FIXME FIXME
// slightly newer but still obsolete code below

class main_window : private gui::window {
private:
    context &m_ctx;
    mode_widget m_mode_widget{*this, gui::layout::fill(), m_ctx};
    gui::menubar m_newmenubar{*this};
    menus::mnu_file m_mnu_file{m_newmenubar, m_ctx};
    menus::mnu_edit m_mnu_edit{m_newmenubar, m_ctx, m_mode_widget};
    menus::mnu_window m_mnu_window{m_newmenubar, m_ctx};

protected:
    void on_close_request() override;

public:
    explicit main_window(context &ctx);

    using window::show;
};

}
}
