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
 * edit_mode_classic.hh
 *
 * The "Classic" editor mode.
 *
 * This file and its associated implementation source files provide an editor
 * mode designed to somewhat mimic the data model presented to the user by the
 * CrashEdit software, another editor developed prior to DRNSF.
 *
 * This mode provides a split view, with a treeview on the left-hand side
 * allowing the user to select an asset, and a more in-depth view containing
 * details of the selected asset on the right-hand side. This is very similar
 * to the Assets mode, however the tree is structured in a fundamentally
 * different manner.
 *
 * The tree displays a single "NSF" asset named "/nsfile", that archive's pages
 * (assuming it is an archive) as its only children, and the entries contained
 * within those pages as their respective children.
 *
 * An example tree is shown here, although the node text may not be accurate:
 *
 *  [-] NSF
 *   |--- [-] Page #0
 *   |     |--- [-] Pagelet #0 - ABC_Z (entry)
 *   |     |--- [-] Pagelet #1 - ABC_S (entry)
 *   |     |--- [-] Pagelet #2 - XYZ_A (entry)
 *   |     \--- [-] Pagelet #3 - 123_M (entry)
 *   |--- [-] Page #1
 *   |     |--- [-] Pagelet #0 - 001gZ (entry)
 *   |     \--- [-] Pagelet #1 - 002gZ (entry)
 *   |--- [+] Page #2
 *   |--- [+] Page #3
 *   |
 *    ...
 *
 * This tree faces some complications not present in CrashEdit:
 *
 *  1. Some objects in the tree may not exist. For example, if the NSF "pages"
 *  list contains an asset name which is not in use. In this case, the object
 *  title is usually displayed as "Nothing".
 *
 *  2. Some objects in the tree may be null references. For example, if a page
 *  "pagelets" list contains a null reference. In this case, the object title
 *  is usually displayed as "Nothing".
 *
 *  3. Some objects in the tree may not have the expected type for their role
 *  within the tree. For example, an entry asset could be referenced directly
 *  by the "pages" list of the NSF, or a "pagelets" element could reference a
 *  non-entry asset such as `gfx::model'. In this case, the object title is
 *  usually displayed as "Incompatible Asset".
 *
 *  4. Some objects in the tree may appear multiple times. For example, an
 *  entry asset could be referenced by multiple pages. In this case, both
 *  nodes in the tree appear independent of eachother, but the editor will
 *  still be displaying and modifying the same asset regardless of which one
 *  is selected.
 *
 * DRNSF will not normally load an NSF file into a state where any of these
 * cases occurs, and most of these scenarios will likely result in a project
 * which cannot be saved to an NSF file again as the structure would be totally
 * incompatible with the NSF file format. Regardless, all of these are valid
 * project configurations in DRNSF, so the editor supports them.
 *
 * Currently, the only difference in this editor as compared to the Assets
 * editor is the tree layout. No differences apply to the asset details view.
 * This will likely change in the future, as the kinds of operations applied in
 * this model (as in CrashEdit) are often different. For example, deleting an
 * entry requires not only deleting the asset, but also some of its referenced
 * assets as well as removing the reference to that asset from its containing
 * page.
 */

#include "edit.hh"

namespace drnsf {
namespace edit {
namespace mode_classic {

/*
 * edit::mode_classic::tree
 *
 * The treeview for the "Classic" mode. This treeview displays an NSF archive
 * and its constituent (by reference) pages and entries. The user can select
 * these assets, which raises an event on the widget.
 */
class tree : private gui::treeview {
private:
    // inner class defined in edit_mode_classic_tree.cc
    class impl;

    // (var) M
    // Pointer to the implementation (PIMPL).
    impl *M;

public:
    // (explicit ctor)
    // Constructs the treeview with the specified layout and container. The tree
    // is initially empty. To populate it, call `set_proj' with a pointer to the
    // project to display the NSF object tree for.
    explicit tree(gui::container &parent, gui::layout layout);

    // (dtor)
    // Destroys the treeview. This may raise the `on_select' event.
    ~tree() noexcept;

    // (func) set_proj
    // Sets the project whose NSF structure should be presented to the user. If
    // the given pointer is null, the tree is cleared completely.
    //
    // The `on_select' event will be raised if there is a selected node when the
    // change occurs.
    void set_proj(res::project *proj);

    using treeview::show;
    using treeview::hide;
    using treeview::get_layout;
    using treeview::set_layout;
    using treeview::get_real_size;
    using treeview::get_screen_pos;

    // (event) on_select
    // Raised when the selected asset has changed. Note that this can be
    // triggered automatically, for example if selected tree node is removed,
    // or if the selected tree node changes to refer to another asset (or no
    // asset at all).
    //
    // This event may be raised during destruction of the tree if the tree has
    // a selected node at that time. Ensure any handler code for this event can
    // handle this case.
    util::event<res::atom> on_select;
};

/*
 * edit::mode_classic::mainctl
 *
 * The main control for the "Classic" mode.
 */
class mainctl : private gui::composite {
private:
    // (var) m_ctx
    // The context this editor is attached to.
    context &m_ctx;

    // (var) m_tree
    // The treeview on the left-hand side of the widget.
    tree m_tree{ *this, gui::layout::grid(0, 1, 3, 0, 1, 1) };

    // (var) m_body
    // The main editor for the tree's selected asset.
    asset_mainctl m_body{ *this, gui::layout::grid(1, 2, 3, 0, 1, 1) };

    // (handler) h_tree_select
    // Hooks the treeview's on_select event to watch for changes in the selected
    // tree node's asset assocation.
    decltype(tree::on_select)::watch h_tree_select;

    // (handler) h_project_change
    // Hooks the editor context's project change event to update the mode for
    // the new target project.
    decltype(context::on_project_change)::watch h_project_change;

public:
    // (explicit ctor)
    // Constructs the main widget and its contents.
    explicit mainctl(
        gui::container &parent,
        gui::layout layout,
        context &ctx);

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;
};

/*
 * edit::mode_classic::handler
 *
 * The mode handler for this mode.
 */
class handler : public mode_handler {
private:
    // (var) m_mainctl
    // The main widget for this mode.
    mainctl m_mainctl;

public:
    // (explicit ctor)
    // Constructs the new mode handler and its widgets.
    explicit handler(gui::container &parent, context &ctx) :
        mode_handler(ctx),
        m_mainctl(parent, gui::layout::fill(), ctx) {}

    // (func) start
    // Displays the editor.
    void start() override
    {
        m_mainctl.show();
    }

    // (func) stop
    // Hides the editor.
    void stop() noexcept override
    {
        m_mainctl.hide();
    }
};

}
}
}
