export interface IRegisterParams {
    username: string;
    email: string;
    password: string;
}

export interface ILoginParams {
    email: string;
    password: string;
    ticket_id?: string;
}

export interface ITokenPayload {
    id: string;
    role: string;
    hwid: string;
}