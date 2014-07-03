/*
 * @file rigid_body.h 
 * @Basic rigid_body class.
 * @author Tianxiang Zhang
 * 
 * This file is part of Physika, a versatile physics simulation library.
 * Copyright (C) 2013 Physika Group.
 *
 * This Source Code Form is subject to the terms of the GNU General Public License v2.0. 
 * If a copy of the GPL was not distributed with this file, you can obtain one at:
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#ifndef PHYSIKA_DYNAMICS_RIGID_BODY_RIGID_BODY_H_
#define PHYSIKA_DYNAMICS_RIGID_BODY_RIGID_BODY_H_

#include "Physika_Core/Transform/transform.h"
#include "Physika_Core/Matrices/matrix_3x3.h"
#include "Physika_Core/Quaternion/quaternion.h"

namespace Physika{

template <typename Scalar,int Dim> class CollidableObject;
template <typename Scalar,int Dim> class Vector;
template <typename Scalar> class SurfaceMesh;

//InertiaTensor is only defined for 3-dimension.
//It can only compute inertia tensor for triangle meshes.
//Support for quad-mesh remains to be implemented.
template <typename Scalar>
class InertiaTensor
{
public:
    InertiaTensor();
    ~InertiaTensor();

    const SquareMatrix<Scalar, 3> bodyInertiaTensor() const;
    SquareMatrix<Scalar, 3> bodyInertiaTensor();
    const SquareMatrix<Scalar, 3> spatialInertiaTensor() const;
    SquareMatrix<Scalar, 3> spatialInertiaTensor();

    //set a body to this inertia tensor. Mesh, scale and density should be provided.
    //mass_center and mass will be modified after calling this function in order to get the center of mass and the value of mass.
    void setBody(SurfaceMesh<Scalar>* mesh, Vector<Scalar, 3> scale, Scalar density, Vector<Scalar, 3>& mass_center, Scalar& mass);

    //give the rotation of this body and get the inertia tensor after rotation.
    //spatial_inertia_tensor_ is modified in this function while body_inertia_tensor_ remains unchanged.
    SquareMatrix<Scalar, 3> rotate(Quaternion<Scalar>& quad);

protected:

    //the inertia tensor of a body referring to its mass center
    //it remains unchanged after setBody()
    SquareMatrix<Scalar, 3> body_inertia_tensor_;

    //the inertia tensor of a body in spatial frame. This is a common used inertia tensor in rigid body simulation
    //it will be modified after calling rotate(Quaternion<Scalar>& quad)
    SquareMatrix<Scalar, 3> spatial_inertia_tensor_;

private:

    /* maximum number of verts per polygonal face */
    const static int MAX_POLYGN_SZ = 3;
    const static int X = 0;
    const static int Y = 1;
    const static int Z = 2;

    inline Scalar SQR(Scalar x){return x * x;};
    inline Scalar CUBE(Scalar x){return x * x * x;};

    struct InertiaTensorFace
    {
        unsigned int numVerts;
        Scalar norm[3];
        Scalar w;
        unsigned int verts[MAX_POLYGON_SZ];
        struct InertiaTensor<Scalar>::InertiaTensorPolyhedron *poly;
    };

    struct InertiaTensorPolyhedron
    {
        unsigned int numVerts;
        unsigned int numFaces;
        Scalar** verts;//numVerts x 3 dimension
        InertiaTensor<Scalar>::InertiaTensorFace* faces;//numFaces dimension
    };

    int A;   /* alpha */
    int B;   /* beta */
    int C;   /* gamma */

    /* projection integrals */
    Scalar P1, Pa, Pb, Paa, Pab, Pbb, Paaa, Paab, Pabb, Pbbb;

    /* face integrals */
    Scalar Fa, Fb, Fc, Faa, Fbb, Fcc, Faaa, Fbbb, Fccc, Faab, Fbbc, Fcca;

    /* volume integrals */
    Scalar T0, T1[3], T2[3], TP[3];

    /* compute various integrations over projection of face */
    void compProjectionIntegrals(InertiaTensorFace *f);
    void compFaceIntegrals(InertiaTensorFace *f);
    void compVolumeIntegrals(InertiaTensorPolyhedron *p);

};

template <typename Scalar,int Dim>
class RigidBody
{
public:
	//constructors && deconstructors
	RigidBody();
	virtual ~RigidBody();

	//get & set
	typename CollidableObject<Scalar, Dim>::ObjectType objectType() const;
	void setMesh(SurfaceMesh<Scalar>* mesh);
	SurfaceMesh<Scalar>* mesh();
	void setTransform(Transform<Scalar>& transform);
	Transform<Scalar> transform() const;
	Transform<Scalar> transform();
	const Transform<Scalar>* transformPtr() const;
	Transform<Scalar>* transformPtr();

	//dynamics
	void update();//update its configuration and velocity

protected:
	//basic properties of a rigid body
	typename CollidableObject<Scalar, Dim>::ObjectType object_type_;
	SurfaceMesh<Scalar>* mesh_;
	Transform<Scalar> transform_;
	Scalar mass_;
	bool is_fixed_;
	
};

} //end of namespace Physika

#endif //PHYSIKA_DYNAMICS_RIGID_BODY_RIGID_BODY_H_
