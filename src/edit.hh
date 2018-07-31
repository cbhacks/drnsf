//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2019  DRNSF contributors
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

/*
 * edit::context
 *
 * FIXME explain
 */
class context : private util::nocopy {
private:
    // (var) m_proj
    // The project currently open under this context, or null if there is no
    // project currently open.
    std::shared_ptr<res::project> m_proj;

public:
    // (explicit ctor)
    // Creates a context with the specified project open. The context takes a
    // copy of the shared pointer.
    explicit context(std::shared_ptr<res::project> proj);

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

    // (event) on_project_change
    // Raised whenever the project is changed by `set_proj'. The previous
    // project, if any, will be kept alive during the execution of this event's
    // handlers, but may be released once the event has finished.
    util::event<const std::shared_ptr<res::project> &> on_project_change;
};

namespace menus {

/*
 * edit::menus::mni_open
 *
 * File -> Open
 * Opens a file. (FIXME what kind?)
 */
class mni_open : private gui::menu::item {
private:
    context &m_ctx;
    void on_activate() final override;

public:
    explicit mni_open(gui::menu &menu, context &ctx) :
        item(menu, "Open"),
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

public:
    explicit mni_save_as(gui::menu &menu, context &ctx) :
        item(menu, "Save As"),
        m_ctx(ctx) {}
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
    mni_open m_open{*this, m_ctx};
    mni_save_as m_save_as{*this, m_ctx};
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
    mni_undo m_undo{*this, m_ctx};
    mni_redo m_redo{*this, m_ctx};

public:
    explicit mnu_edit(gui::menubar &menubar, context &ctx) :
        item(menubar, "Edit"),
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

        using std::to_string;
        ImGui::Text(to_string(obj).c_str());
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

        using std::to_string;
        ImGui::Text(to_string(obj).c_str());
    }

    // (event) on_change
    // See the non-specialized `edit::field' for details.
    util::event<T> on_change;
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

        ImGui::Text(to_string(obj).c_str());
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

        ImGui::Text("List of $"_fmt(obj.size()).c_str());
        ImGui::NextColumn();
        ImGui::NextColumn();

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
        } else {
            ImGui::Text("--");
        }

        // Display the inner field and zero-based index on the next row.
        ImGui::NextColumn();
        ImGui::Indent();
        ImGui::Text("[$]"_fmt(m_index).c_str());
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
 * edit::asset_editor
 *
 * FIXME explain
 */
class asset_editor : private gui::composite {
private:
    // inner class defined in edit_asset_editor.cc
    class impl;

    // (var) M
    // The pointer to the internal implementation object (PIMPL).
    impl *M;

public:
    // (ctor)
    // Constructs the widget and places it in the given parent container.
    asset_editor(gui::container &parent, gui::layout layout, res::project &proj);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~asset_editor();

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
    asset_tree(gui::container &parent, gui::layout layout, res::project &proj);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~asset_tree();

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
 * edit::map_mainctl
 *
 * FIXME explain
 */
class map_mainctl : private render::viewport {
private:
    // (var) m_ctx
    // The context this map editor is attached to.
    context &m_ctx;

    // (var) m_reticle
    // FIXME explain
    render::reticle_fig m_reticle{*this};

    // (var) m_world_figs
    // FIXME explain
    std::unordered_map<
        gfx::world *,
        std::unique_ptr<render::world_fig>> m_world_figs;

    // (handler) h_asset_appear, h_asset_disappear
    // Hooks the editor context's current project to see when assets come into
    // and out of existence so their figures can be added or removed on the
    // viewport.
    decltype(res::project::on_asset_appear)::watch h_asset_appear;
    decltype(res::project::on_asset_disappear)::watch h_asset_disappear;

public:
    // (explicit ctor)
    // FIXME explain
    explicit map_mainctl(
        gui::container &parent,
        gui::layout layout,
        context &ctx);

    using viewport::show;
    using viewport::hide;
    using viewport::get_layout;
    using viewport::set_layout;
    using viewport::get_real_size;
    using viewport::get_screen_pos;
};

// FIXME - temporary global for compatibility
extern res::atom g_selected_asset;

// FIXME FIXME FIXME FIXME FIXME
// slightly newer but still obsolete code below

class main_window : private gui::window {
private:
    context &m_ctx;
    gui::menubar m_newmenubar{*this};
    menus::mnu_file m_mnu_file{m_newmenubar, m_ctx};
    menus::mnu_edit m_mnu_edit{m_newmenubar, m_ctx};
    asset_editor m_assets_view{
        *this,
        gui::layout::grid(0, 2, 3, 0, 1, 1),
        *m_ctx.get_proj()};
    map_mainctl m_map_view{
        *this,
        gui::layout::grid(2, 1, 3, 0, 1, 1),
        m_ctx};

protected:
    void on_close_request() override;

public:
    explicit main_window(context &ctx);

    using window::show;
};

//// SEEN BELOW: soon-to-be-obsolete code ////

class im_canvas : public gui::widget_im {
private:
    void frame() final override;

public:
    im_canvas(gui::container &parent, gui::layout layout) :
        widget_im(parent, layout) {}

    using widget_im::show;

    util::event<int, int, int> on_frame;
};

using res::project;

class core; // FIXME

class old_editor;

class main_view : private util::nocopy {
    friend class core; // FIXME

    friend class pane;

private:
    old_editor &m_ed;
    gui::window m_canvas_wnd;
    im_canvas m_canvas;
    decltype(m_canvas.on_frame)::watch h_frame;

public:
    explicit main_view(old_editor &ed);

    void frame(int delta_time);

    void show();
};

class pane;
class mode;

class old_editor : private util::nocopy {
    friend class main_view;
    friend class pane;

private:
    project &m_proj;
    std::list<pane *> m_panes;
    std::unique_ptr<mode> m_mode;

public:
    explicit old_editor(project &proj);

    project &get_project() const;
};

class pane : private util::nocopy {
private:
    std::string m_id;
    decltype(old_editor::m_panes)::iterator m_iter;

protected:
    old_editor &m_ed;

public:
    explicit pane(old_editor &ed, std::string id);
    ~pane();

    virtual void show() = 0;

    const std::string &get_id() const;
    virtual std::string get_title() const = 0;
};

class mode : private util::nocopy {
protected:
    old_editor &m_ed;

    explicit mode(old_editor &ed) :
        m_ed(ed) {}

public:
    virtual ~mode() = default;

    virtual void update(double delta_time) {}
    virtual void render() {}
    virtual void show_gui() {}
};

class modedef : private util::nocopy {
private:
    std::string m_title;

protected:
    explicit modedef(std::string title);
    ~modedef();

public:
    static const std::set<modedef*> &get_list();

    const std::string &get_title() const
    {
        return m_title;
    }

    virtual std::unique_ptr<mode> create(old_editor &ed) const = 0;
};

template <typename T>
class modedef_of : private modedef {
public:
    std::unique_ptr<mode> create(old_editor &ed) const override
    {
        return std::unique_ptr<mode>(new T(ed));
    }

    explicit modedef_of(std::string title) :
        modedef(title) {}
};

class core : private util::nocopy {
public:
    project &m_proj;
    old_editor m_ed;
    edit::main_view m_wnd;

    core(project &proj);
};

}
}
