// To see this in action, run this in a terminal:
//      gp preview $(gp url 8000)

import * as React from "react";
import * as ReactDOM from "react-dom";
import { Api, JsonRpc, RpcError } from 'eosjs';
import { JsSignatureProvider } from 'eosjs/dist/eosjs-jssig';

const rpc = new JsonRpc(''); // nodeos and web server are on same port

interface PostData {
    id?: number;
    user?: string;
    reply_to?: number;
    content?: string;
    likes?: number;
    dislikes?: number;
};

interface PostFormState {
    privateKey: string;
    data: PostData;
    error: string;
};

class PostForm extends React.Component<{}, PostFormState> {
    api: Api;

    constructor(props: {}) {
        super(props);
        this.api = new Api({ rpc, signatureProvider: new JsSignatureProvider([]) });
        this.state = {
            privateKey: '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',
            data: {
                id: 0,
                user: 'bob',
                reply_to: 0,
                content: 'This is a test',
                likes: 0,
                dislikes: 0
            },
            error: '',
        };
    }

    setData(data: PostData) {
        this.setState({ data: { ...this.state.data, ...data } });
    }

    async post() {
        try {
            this.api.signatureProvider = new JsSignatureProvider([this.state.privateKey]);
            const result = await this.api.transact(
                {
                    actions: [{
                        account: 'talk',
                        name: 'post',
                        authorization: [{
                            actor: this.state.data.user,
                            permission: 'active',
                        }],
                        data: this.state.data,
                    }]
                }, {
                    blocksBehind: 3,
                    expireSeconds: 30,
                });
            console.log(result);
            this.setState({ error: '' });
        } catch (e) {
            if (e.json)
                this.setState({ error: JSON.stringify(e.json, null, 4) });
            else
                this.setState({ error: '' + e });
        }
    }

    render() {
        //Attention: The input boxes ReplyTo, Likes and DisLikes look readable only, BUT they are actually editable !
        //           You shall move your cursor ***BEFORE 0 to type, then you can enter a value like 81
        return <div>
            <table>
                <tbody>
                    <tr>
                        <td>Private Key</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.privateKey}
                            onChange={e => this.setState({ privateKey: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>User</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.user}
                            onChange={e => this.setData({ user: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>Reply To</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.reply_to}
                            onChange={e => this.setData({ reply_to: +e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>Content</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.content}
                            onChange={e => this.setData({ content: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>Likes</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.likes}
                            onChange={e => this.setData({ likes: +e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>Dislikes</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.dislikes}
                            onChange={e => this.setData({ dislikes: +e.target.value })}
                        /></td>
                    </tr>
                </tbody>
            </table>
            <br />
            <button onClick={e => this.post()}>Post</button>
            {this.state.error && <div>
                <br />
                Error:
                <code><pre>{this.state.error}</pre></code>
            </div>}
        </div>;
    }
}

class Messages extends React.Component<{}, { content: string }> {
    interval: number;

    constructor(props: {}) {
        super(props);
        this.state = { content: '///' };
    }

    componentDidMount() {
        this.interval = window.setInterval(async () => {
            try {
                const rows = await rpc.get_table_rows({
                    json: true, code: 'talk', scope: '', table: 'message', limit: 1000,
                });
                let content =
                    'id          reply_to      likes      dislikes      user          content\n' +
                    '===============================================================================\n';
                for (let row of rows.rows)
                    content +=
                        (row.id + '').padEnd(12) +
                        (row.reply_to + '').padEnd(12) + '  ' +
                        (row.likes + '').padEnd(12) + '  ' +
                        (row.dislikes + '').padEnd(12) + '  ' +
                        row.user.padEnd(14) +
                        row.content + '\n';
                this.setState({ content });
            } catch (e) {
                if (e.json)
                    this.setState({ content: JSON.stringify(e.json, null, 4) });
                else
                    this.setState({ content: '' + e });
            }

        }, 200);
    }

    componentWillUnmount() {
        clearInterval(this.interval);
    }

    render() {
        return <code><pre>{this.state.content}</pre></code>;
    }
}

ReactDOM.render(
    <div>
        <PostForm />
        <br />
        Messages:
        <Messages />
    </div>,
    document.getElementById("example")
);
