<?xml version="1.0" encoding="UTF-8"?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns
     http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
    <key id="l2" for="node" attr.name="l2" attr.type="string"/>
    <key id="l2:Age" for="node" attr.name="l2:Age" attr.type="double"/>
    <key id="l1" for="node" attr.name="l1" attr.type="string"/>
    <key id="l1:Color" for="node" attr.name="l1:Color" attr.type="string"/>
    <key id="l1:Age" for="node" attr.name="l1:Age" attr.type="double"/>
    <key id="v_name" for="node" attr.name="name" attr.type="string"/>
    <key id="e_type" for="edge" attr.name="e_type" attr.type="string"/>
    <key id="el2-l2: Weight" for="edge" attr.name="l2-l2: Weight" attr.type="double"/>
    <key id="el2-l2: Stars" for="edge" attr.name="l2-l2: Stars" attr.type="double"/>
    <key id="el1-l1: Stars" for="edge" attr.name="l1-l1: Stars" attr.type="double"/>
  <graph id="mlnet 3" edgedefault="directed">
    <node id="2">
        <data key="v_name">U5:l2</data>
        <data key="l2:Age">32</data>
    </node>
    <node id="6">
        <data key="v_name">U1:l2</data>
        <data key="l2:Age">0</data>
    </node>
    <node id="7">
        <data key="v_name">U3:l2</data>
        <data key="l2:Age">0</data>
    </node>
    <node id="8">
        <data key="v_name">U4:l2</data>
        <data key="l2:Age">0</data>
    </node>
    <node id="1">
        <data key="v_name">U0:l1</data>
        <data key="l1:Color">Blue</data>
        <data key="l1:Age">34</data>
    </node>
    <node id="3">
        <data key="v_name">U1:l1</data>
        <data key="l1:Color"></data>
        <data key="l1:Age">0</data>
    </node>
    <node id="4">
        <data key="v_name">U2:l1</data>
        <data key="l1:Color"></data>
        <data key="l1:Age">0</data>
    </node>
    <node id="5">
        <data key="v_name">U3:l1</data>
        <data key="l1:Color"></data>
        <data key="l1:Age">0</data>
    </node>
    <key id="e_type" for="edge" attr.name="e_type" attr.type="string"/>
    <edge id="e5" source="6" target="7">
        <data key="e_type">l2-l2</data>
        <data key="el2-l2: Weight">3</data>
        <data key="el2-l2: Stars">4</data>
    </edge>
    <edge id="e6" source="6" target="8">
        <data key="e_type">l2-l2</data>
        <data key="el2-l2: Weight">4</data>
        <data key="el2-l2: Stars">5</data>
    </edge>
    <edge id="e7" source="7" target="8">
        <data key="e_type">l2-l2</data>
        <data key="el2-l2: Weight">2</data>
        <data key="el2-l2: Stars">5</data>
    </edge>
    <edge id="e1" source="1" target="3">
        <data key="e_type">l1-l1</data>
        <data key="el1-l1: Stars">4</data>
    </edge>
    <edge id="e2" source="1" target="4">
        <data key="e_type">l1-l1</data>
        <data key="el1-l1: Stars">4</data>
    </edge>
    <edge id="e3" source="3" target="4">
        <data key="e_type">l1-l1</data>
        <data key="el1-l1: Stars">5</data>
    </edge>
    <edge id="e4" source="3" target="5">
        <data key="e_type">l1-l1</data>
        <data key="el1-l1: Stars">4</data>
    </edge>
  </graph>
</graphml>
