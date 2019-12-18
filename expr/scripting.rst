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
