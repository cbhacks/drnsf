This is an experimental documentation file for the DRNSF scripting API. It may
be completely rewritten, reformatted, or scrapped in the future.

TBD

Legend
------

*(read-only)*
  * For attributes: the attribute cannot be set.


Atoms
=====

TBD

Atom objects may be compared using ``==`` or ``!=``. Avoid using ``is`` or
``is not``. Most built-in functions and attributes return a new Atom object at
each invocation or reference.

Atom objects support path building using the "true division" operator ``/``.
For example, given a project ``p``, both ``Atom('/A/B/C', p)`` and
``p.root / 'A' / 'B' / 'C'`` are equivalent.

.. py:class:: Atom(path, project)

   * *path* :py:class:`str`
   * *project* :py:class:`Project`

   Constructs an atom referring to the node at *path* within *project*. If one
   does not exist, it is created. If *path* is an empty string, constructs an
   atom referring to the root node of *project*.

   .. py:attribute:: parent

      *(read-only)* :py:class:`Atom` or ``None``

      An atom referring to this atom's node's parent node.

      For example, the node ``/A/B/C/D`` has parent node ``/A/B/C``, so
      ``Atom('/A/B/C/D', p).parent == Atom('/A/B/C', p)``.

      For a root node (:py:attr:`Project.root`), this is always ``None``.

   .. py:attribute:: basename

      *(read-only)* :py:class:`str`

      The name of the node referred to by this atom, excluding the path of its
      parent. For example, an atom referring to the node ``/A/B/C/D`` has
      basename ``'D'``.

      For a root node (:py:attr:`Project.root`), this is an empty string.

   .. py:attribute:: dirname

      *(read-only)* :py:class:`str`

      The path of this atom's node's parent. For example, an atom referring to
      the node ``/A/B/C/D`` has dirname ``'/A/B/C'``.

      For a root node (:py:attr:`Project.root`), this is an empty string.

   .. py:attribute:: path

      *(read-only)* :py:class:`str`

      The path of this atom's node. For example, an atom referring to the node
      ``/A/B/C/D`` has path ``'/A/B/C/D'``.

      For a root node (:py:attr:`Project.root`), this is an empty string.

   .. py:attribute:: asset

      *(read-only)* :py:class:`Asset`-derived type or ``None``

      The asset whose name is the node associated with this atom. For example,
      an asset whose name is ``/A/B/C/D`` could be reached from it's project
      root via ``(project.root // '/A/B/C/D').asset``.

      If this name is not associated with an asset, returns ``None``.

   .. py:function:: firstchild()

      Returns :py:class:`Atom` or ``None``

      Returns an atom referring to the first child node of this atom's node, or
      ``None`` if there are no children.

   .. py:function:: nextsibling()

      Returns :py:class:`Atom` or ``None``

      Returns an atom referring to the next sibling node of this atom's node;
      that is, the next node which shares the same parent. Returns ``None`` if
      this atom's node is the last or only child of its parent node.

      For a root node (:py:attr:`Project.root`), this always returns ``None``.

   .. py:function:: eachchild()

      Returns a generator which yields an atom for each child node of this
      atom's node. Atoms are returned in lexicographical order. The addition or
      removal of child nodes during enumeration is allowed.

   .. py:function:: eachdescendant()

      Returns a generator which yields an atom for each descendant node of this
      atom's node. Atoms are returned in lexicographical order. The addition or
      removal of descendant nodes during enumeration is allowed.

   .. py:function:: eachatom()

      Returns a generator which yields this Atom as well as an atom for each
      descendant node. Descendant nodes are enumerated the same as in
      :py:func:`eachdescendant`.

   .. py:function:: eachasset(basetype=Asset)

      Returns a generator which yields the asset associated with this atom's
      node as well as each of the descendant nodes. Nodes with no associated
      asset are excluded, but their descendants are still considered.

      An optional ``basetype`` argument may be provided, in which case only the
      assets of the given type or derived from that type are yielded.

.. py:function:: eachatom()

   Returns a generator which yields an Atom for each node in the "active"
   project's asset name tree, if any. This is the same as calling
   *P().eachatom()*.

   If there is no active project, instead returns a generator which yields
   no values.


Projects
========

TBD

At any point in time, one project may be the "active" project. Projects may be
pushed onto a "project stack" to become the new active project, and then later
popped to restore the previous active project. When the project stack is empty,
the active project is whatever project is opened in the application UI.

The project stack may be modified directly by the :py:func:`pushproject` and
:py:func:`popproject` functions, or by using a project object in a *with* block
as shown below:

::

  print(P())       # prints the UI project
  with Project():
      print(P())   # prints the new project created above
  print(P())       # prints the UI project, again

.. py:class:: Project()

   Constructs a new empty project.

   .. py:attribute:: root

      *(read-only)* :py:class:`Atom`

      The root atom of the project. All assets in a project have a name that is
      a descendant of the project's root atom.

      The root atom cannot be the name of an asset.

   .. py:function:: eachatom()

      Returns a generator which yields an Atom for each node in the project's asset
      name tree. This is the same as calling *root.eachatom()*.

   .. py:function:: eachasset(basetype=Asset)

      Returns a generator which yields each asset in the project.

      An optional ``basetype`` argument may be provided, in which case only the
      assets of the given type or derived from that type are yielded.

.. py:function:: P()

   Returns :py:class:`Project` or ``None``

   Returns the "active" project. This is the top project of the project stack, or
   the result of :py:func:`getcontextproject` if the project stack is empty.

.. py:function:: getcontextproject()

   Returns :py:class:`Project` or ``None``

   Returns the project which is currently open in the user interface. Returns
   ``None`` if there is no open project.

.. py:function:: pushproject(project)

   * *project* :py:class:`Project`

   Pushes *project* onto the project stack.

.. py:function:: popproject()

   Pops the top project from the project stack. An error occurs if the project
   stack is empty.


Assets
======

TBD

.. py:class:: Asset

   Cannot be constructed.

   An asset may become invalid at any time, for example if the asset is deleted
   by the user or a script, or if the user undoes the transaction which created
   the asset. Such an asset is unusable by any script; any access to any of the
   below-described attributes or calling any of the below-described methods on
   such an invalid asset will raise an error.

   .. py:attribute:: project

      *(read-only)* :py:class:`Project`

      The project which this asset is a member of. This never changes for the
      entire lifetime of the asset.

   .. py:attribute:: name

      *(read-only)* :py:class:`Atom`

      The name associated with this Asset. The returned atom's ``project`` and
      this asset's ``project`` are identical.

.. py:function:: eachasset(basetype=Asset)

   Returns a generator which yields each asset in the "active" project, if any.

   If there is no active project, instead returns a generator which yields
   no values.


Transactions
============

TBD

.. py:function:: begin()

   Begins a new transaction with the active project. This pushes the active
   project to the project stack, as if by ``pushproject(P())``. During the
   lifetime of the transaction, the project stack may not be manipulated. The
   transaction is terminated by calling ``commit()`` or ``rollback()``.

   If this function is called while there is no active project, an error is
   raised.

   If this function is called while there is a transaction already in progress,
   an error is raised.

   During the transaction's lifetime, the user is prohibited from performing
   any other transaction-based operations such as modifying asset properties
   or influencing the asset lifecycles.

.. py:function:: commit()

   Commits all of the changes made during the transaction, and terminates the
   transaction. The changes within the transaction become a named work unit in
   the user's Undo/Redo listing, and the user may undo the transaction at any
   point in the future.

   This also pops the project from the project stack which was pushed by the
   ``begin()`` function.

   If this function is called while there is no running transaction, an error
   is raised.

   (TBD error if attempting to commit foreign or managed transaction ?)

.. py:function:: rollback()

   Reverts all of the changes made during the transaction, and terminates the
   transaction. The Undo/Redo listing is unaffected.

   This also pops the project from the project stack which was pushed by the
   ``begin()`` function.

   If this function is called while there is no running transaction, an error
   is raised.

   (TBD error if attempting to commit foreign or managed transaction ?)
